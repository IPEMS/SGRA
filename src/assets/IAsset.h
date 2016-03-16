/*
 * IAsset.h
 *
 *  Created on: Dec 3, 2013
 *      Author: hansentm
 */

#ifndef IASSET_H_
#define IASSET_H_

class IAsset {
public:
	double getAveragePower(int time);
	virtual ~IAsset();
private:
	IAsset();

};

/*
 * if there is a seeding function, inherit this.
class IAssetGAControllerSeed{
public:
	IAssetGAControllerSeed();
	virtual ~IAssetGAControllerSeed();
private:

};
*/

/*
 * interface to control the assets.  each asset needs a controller that inherits this to interface with the GA.
 *
 * GA <- IAssetGAController <- asset list -> IAssetGAController -> specific asset
 *
 * all control aspects of the IAssetGAController are implemented locally.  GA does not need to know.
 */
class IAssetGAController {
public:
	//void seed(IAssetGAControllerSeed seed)=0; //seeded init state
	virtual ~IAssetGAController();
	void mutate()=0; //mutate
private:
	IAssetGAController(); //random init state
};

#endif /* IASSET_H_ */
