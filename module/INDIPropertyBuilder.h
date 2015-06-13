/*
 * INDIPropertyBuilder.h
 *
 *  Created on: May 29, 2014
 *      Author: klaus
 */
// Copyright (c) 2013-2015, Klaus Kretzschmar. All Rights Reserved.

#ifndef INDIPROPERTYBUILDER_H_
#define INDIPROPERTYBUILDER_H_

#include <pcl/String.h>
#include "IINDIProperty.h"

namespace pcl {

class INDIPropertyBuilder {
public:
	INDIPropertyBuilder(INDI_TYPE t);
	virtual ~INDIPropertyBuilder(){}

	INDIPropertyBuilder& device(IsoString device);
	INDIPropertyBuilder& property(IsoString device);
	INDIPropertyBuilder& addElement(IsoString elementName,IsoString value);

	IProperty* getProperty(){return m_property;}
private:
	IProperty* m_property;
};

} /* namespace pcl */
#endif /* INDIPROPERTYBUILDER_H_ */
