/*
 * PixInsightINDIParamListTypes.h
 *
 *  Created on: May 26, 2014
 *      Author: klaus
 */

#ifndef PIXINSIGHTINDIPARAMLISTTYPES_H_
#define PIXINSIGHTINDIPARAMLISTTYPES_H_

#include "indibase.h"


namespace pcl{

struct INDIDeviceListItem {
	INDIDeviceListItem(){}
	String DeviceName;
	String DeviceLabel;
	bool operator==(const INDIDeviceListItem& rhs ) const {
		return (DeviceName == rhs.DeviceName);
	}
};

typedef enum {
	Insert,
	Update,
	Remove,
	Idle
} PropertyFlagType;

struct INDIPropertyListItem {
	INDIPropertyListItem():PropertyFlag(Idle){}
	String Device;
	String Property;
	INDI_TYPE PropertyType;
	String PropertyTypeStr;
	String Element;
	unsigned int PropertyState;
	PropertyFlagType   PropertyFlag;
	String PropertyNumberFormat;
	double numberMin;
	double numberMax;
	double numberStep;
	String PropertyLabel;
	String ElementLabel;
	String PropertyKey;
	String PropertyValue;
	String NewPropertyValue;
	bool operator==(const INDIPropertyListItem& rhs) const {
		return (PropertyKey == rhs.PropertyKey) ;
	}
};

struct INDINewPropertyListItem {
	INDINewPropertyListItem(){}
	String Device;
	String Property;
	String PropertyType;
	String Element;
	String PropertyKey;
	String NewPropertyValue;
	bool operator==(const INDINewPropertyListItem& rhs) const;
};
}


#endif /* PIXINSIGHTINDIPARAMLISTTYPES_H_ */
