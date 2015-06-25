/*
 * SkyMap.cpp
 *
 *  Created on: Jun 21, 2015
 *      Author: klaus
 */

#include "SkyMap.h"
#include "pcl/Console.h"

namespace pcl {

// ----------------------------------------------------------------------------

CoordUtils::coord_HMS CoordUtils::convertToHMS(double coord){
	coord_HMS coordInHMS;
	coordInHMS.hour   = trunc(coord);
	coordInHMS.minute = trunc((coord - coordInHMS.hour) * 60);
	coordInHMS.second = trunc(((coord - coordInHMS.hour) * 60 - coordInHMS.minute ) * 60*100)/100.0;
	return coordInHMS;
}

CoordUtils::coord_HMS CoordUtils::parse(IsoString coordStr){
	CoordUtils::coord_HMS result;
	StringList tokens;
	coordStr.Break(tokens,':',true);
	tokens[0].TrimLeft();
	result.hour=tokens[0].ToDouble();
	result.minute=tokens[1].ToDouble();
	result.second=tokens[2].ToDouble();
	return result;
}

double CoordUtils::convertFromHMS(const CoordUtils::coord_HMS& coord_in_HMS){
	int sign=coord_in_HMS.hour >= 0 ? 1 : -1;
	// seconds are rounded to two fractional decimals
	double coord=sign * (fabs(coord_in_HMS.hour) + coord_in_HMS.minute / 60 + coord_in_HMS.second / 3600);
	return coord;
}

double CoordUtils::convertDegFromHMS(const CoordUtils::coord_HMS& coord_in_HMS){
	return CoordUtils::convertFromHMS(coord_in_HMS) * 360.0 / 24.0 ;
}

double CoordUtils::convertRadFromHMS(const CoordUtils::coord_HMS& coord){
	return CoordUtils::convertDegFromHMS(coord) * c_rad;
}


StereoProjection::polarCoord StereoProjection::project(const StereoProjection::sphericalCoord& spherical){
	polarCoord result;

	result.r = Sin(spherical.theta) / (1.0 - Cos(spherical.theta));
	result.phi = spherical.phi;

	return result;
}

StereoProjection::polarCoord StereoProjection::projectScaled(const StereoProjection::sphericalCoord& spherical,double scale){
	polarCoord result;

	result.r = scale * Sin(spherical.theta) / (1.0 - Cos(spherical.theta));
	result.phi = spherical.phi;

	return result;
}


StereoProjection::euklidCoord StereoProjection::polarToEuklid(StereoProjection::polarCoord& polar){

	euklidCoord result;
	result.x = polar.r * Sin(-polar.phi);
	result.y = polar.r * Cos(-polar.phi);
	return result;
}



SkyMap::SkyMap(const SkyMap::filter& filter,const SkyMap::geoCoord& geoCoord):m_filter(filter), m_geoCoord(geoCoord) {


}

IsoString  SkyMap::getASDLQueryString(){

	IsoString queryStr ("SELECT  main_id AS \"Main identifier\",  RA      AS  \"RA\", DEC     AS  \"DEC\", v       AS  \"V\", sp_type as \"Spectral Type\"  FROM basic JOIN mesUBV ON mesUBV.oidref = basic.oid WHERE v < %f and mespos = 1 and DEC > %f and DEC < %f ORDER BY v ASC;");

	return IsoString().Format(queryStr.c_str(),m_filter.v_upperLimit,m_filter.dec_lowerLimit,m_filter.dec_upperLimit);
}

IsoString SkyMap::getASDLFoVQueryString(coord_d ra_center, coord_d dec_center, double width, double height, double limitStarMag){
	IsoString queryStr ("SELECT  main_id AS \"Main identifier\",  RA      AS  \"RA\", DEC     AS  \"DEC\", flux       AS  \"flux\",filter       AS  \"filter\", sp_type as \"Spectral Type\"  FROM basic JOIN flux ON flux.oidref = basic.oid WHERE flux < %f and filter = 'V' and CONTAINS(POINT('ICRS',RA,DEC),BOX('ICRS',%f,%f,%f,%f)) = 1 ORDER BY flux ASC;");
	return IsoString().Format(queryStr.c_str(),limitStarMag,ra_center,dec_center,width,height);
}

double SkyMap::getObjectAltitude(double dec, double hourAngle, double geo_lat){
	return ArcSin( Cos(Rad(geo_lat)) * Cos(Rad(dec)) * Cos(Rad(hourAngle)) + Sin(Rad(geo_lat)) * Sin(Rad(dec))  ) / Pi() * 180.0;
}

double SkyMap::getObjectAzimut(double dec, double ha, double lat){
	double rdec = Rad(dec);
	double rha  = Rad(ha);
	double rlat = Rad(lat);

	double x = Cos(rdec) * Cos(rha) * Sin(rlat) - Sin(rdec) * Cos(rlat);
	double y = Cos(rdec) * Sin(rha);
	return ArcTan2Pi(y,x) / Pi() *180.0;
}



RGBA  SkyMap::getStarColor(IsoString spType){

	if (spType.BeginsWith('O')){
		return RGBAColor(255, 255, 255); // white
	} else if (spType.BeginsWith('B')){
		return RGBAColor(153, 153, 153); // grey
	} else if (spType.BeginsWith('A')){
		return RGBAColor(0, 0, 153); // blue
	} else if (spType.BeginsWith('F')){
		return RGBAColor(0, 153, 153); // gren-blue
	} else if (spType.BeginsWith('G')){
		return RGBAColor(0, 153, 0); // green
	} else if (spType.BeginsWith('K')){
		return RGBAColor(153, 153, 0); // yellow
	} else if (spType.BeginsWith('M')){
		return RGBAColor(153, 0, 0); // red
	}
	return RGBAColor(153, 153, 0);

}

void SkyMap::plotStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics g, double limitStarMag){

	for (SkyMap::ObjectListType::iterator iter = m_objectList.Begin();iter != m_objectList.End(); ++iter) {
		// draw stars
		double hourAngle = iter->ra - lst * 360.0 / 24.0;
		double currentAlt = getObjectAltitude(iter->dec,hourAngle,geo_lat);
		double currentAz = SkyMap::getObjectAzimut(iter->dec, hourAngle, geo_lat);
		if (currentAlt > 3) {
			StereoProjection::sphericalCoord spherical;
			spherical.phi = Rad(currentAz);
			spherical.theta = Rad(90.0 + currentAlt);
			StereoProjection::polarCoord polarCoord = 	StereoProjection::projectScaled(spherical,scale);
			double xs = StereoProjection::polarToEuklid(polarCoord).x;
			double ys = StereoProjection::polarToEuklid(polarCoord).y;
			Brush brush(getStarColor(iter->spType));
			double starRadius = Sqrt(0.5 * Pow(2.5, limitStarMag - iter->v));
			g.FillCircle(x + xs, y + ys, starRadius, brush);
		}
	}

}

void SkyMap::plotFoVStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics g, double limitStarMag){

	for (SkyMap::ObjectListType::iterator iter = m_fovObjectList.Begin();iter != m_fovObjectList.End(); ++iter) {
		// draw stars
		double hourAngle = iter->ra;
		double currentAlt = getObjectAltitude(iter->dec,hourAngle,geo_lat);
		double currentAz = SkyMap::getObjectAzimut(iter->dec, hourAngle, geo_lat);

		StereoProjection::sphericalCoord spherical;
		spherical.phi = Rad(currentAz);
		spherical.theta = Rad(90.0 + currentAlt);
		StereoProjection::polarCoord polarCoord = 	StereoProjection::projectScaled(spherical,scale);
		double xs = StereoProjection::polarToEuklid(polarCoord).x;
		double ys = StereoProjection::polarToEuklid(polarCoord).y;
		Brush brush(getStarColor(iter->spType));
		double starRadius = Sqrt(0.5 * Pow(2.5, limitStarMag - iter->v));
		//Console().WriteLn(String().Format("x=%f, y=%f, r=%f",x+xs,y+ys,scale));
		g.FillCircle(x + xs, y + ys, starRadius > 15 ?  15 : starRadius , brush);

	}

}

} /* namespace pcl */
