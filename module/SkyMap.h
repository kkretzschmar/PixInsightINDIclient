/*
 * SkyMap.h
 *
 *  Created on: Jun 21, 2015
 *      Author: klaus
 */

#ifndef SKYMAP_H_
#define SKYMAP_H_

#include "pcl/Array.h"
#include "pcl/String.h"
#include "pcl/Math.h"
#include "pcl/Graphics.h"

namespace pcl {

const double c_rad = Pi() / 180.0;

class CoordUtils {
public:
	typedef struct {
		double   hour;
		double   minute;
		double   second;
	} coord_HMS;

	static coord_HMS convertToHMS(double coord);
	static coord_HMS parse(IsoString coordStr);
	static double convertFromHMS(const coord_HMS& coord);
	static double convertDegFromHMS(const coord_HMS& coord);
	static double convertRadFromHMS(const coord_HMS& coord);

};

class StereoProjection {
public:
	typedef struct {
		double phi;   //
		double theta;
	} sphericalCoord;

	typedef struct {
		double   r;   //
		double   phi;
	} polarCoord;

	typedef struct {
		double x;
		double y;
	} euklidCoord;

	static polarCoord project(const sphericalCoord& azimutalCoord);
	static polarCoord projectScaled(const sphericalCoord& azimutalCoord,double scale);
	static euklidCoord polarToEuklid(polarCoord& polar);
};

class SkyMap {
public:

	typedef struct {
		IsoString mainId;   // main identifier
		double    ra;       // right ascension
		double    dec;      // declination
		double    v;        // visual apparent luminosity
		IsoString spType;   // spectral type
	} object;

	typedef Array<object> ObjectListType;

	typedef struct {
		double dec_upperLimit; // upper dec limit of visible object
		double dec_lowerLimit; // lower dec limit of visible object
		double v_upperLimit;   // upper limit of visual apparent luminosity
		//IsoString spType;   // spectral type
	} filter;

	typedef struct {
		double geo_lat;    // geographical latitude
		double geo_long;   // geographical longitude
	} geoCoord;

	typedef double coord_h; // sperical coord in hours, e.g. 21.23412
	typedef double coord_d;  // spercial coordinate in degrees, e.g. 342.34678

	SkyMap(const filter& filter, const geoCoord& geoCoord);
	virtual ~SkyMap(){}

	// get object list of current sky map
	ObjectListType* getObjectList()
	{
		return &m_objectList;
	}

	// get object list of current sky map
	ObjectListType* getFoVObjectList()
	{
		return &m_fovObjectList;
	}

	void addObject(const object& object){
		m_objectList.Add(object);
	}

	void addObjectToFoV(const object& object){
		m_fovObjectList.Add(object);
	}

	void clearFoVObjectList(){
		m_fovObjectList.Clear();
	}

	IsoString getASDLQueryString();
	IsoString getASDLFoVQueryString(coord_d ra_center, coord_d dec_center, double width, double height, double limitStarMag);

	static double getObjectAltitude(coord_d dec, coord_d hourAngle, coord_d geo_lat);
	static double getObjectAzimut(coord_d dec, coord_d hourAngle, coord_d geo_lat);
	void plotStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics g, double limitStarMag);
	void plotFoVStars(coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics g, double limitStarMag);

	static RGBA  getStarColor(IsoString spType);

private:
	ObjectListType m_objectList;
	ObjectListType m_fovObjectList;
	filter         m_filter;
	geoCoord       m_geoCoord;
};

} /* namespace pcl */

#endif /* SKYMAP_H_ */
