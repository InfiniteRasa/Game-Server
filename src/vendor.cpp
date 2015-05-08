#include "global.h"


/*
 * jobdata->outVendorList: contains a list with vendor specific data like appearance (and later vendor data, auctioneer data etc.)
 * It is important that NPC data is loaded after creature types (npc data extends creature data)
 */
sint8 vendorLoadState;
void _cb_vendor_init(void *param, diJob_vendorListData_t *jobData)
{
	for(sint32 i=0; i<jobData->outVendorCount; i++)
	{
		di_vendorData_t *vendorData = jobData->outVendorList+i;
		// find the creature type
		creatureType_t* creatureType = creatureType_findType(vendorData->creatureTypeId);
		if( creatureType == NULL )
		{
			printf("Failed to find creature type %d for vendor data\n", vendorData->creatureTypeId);
			break;
		}
		// allocate and init vendor data
		vendorData_t* vendor = (vendorData_t*)malloc(sizeof(vendorData_t));
		memset(vendor, 0x00, sizeof(vendorData_t));
		creatureType->vendorData = vendor;
		// set vendor data
		vendor->vendorPackageId = vendorData->vendorPackageId;
		// init sold item list
		if( vendorData->numberOfItems > 0 )
		{
			// todo: Replace the item list with a vector
			vendor->numberOfSoldItems = vendorData->numberOfItems;
			vendor->soldItemList = (vendorItemEntry_t*)malloc(sizeof(vendorItemEntry_t) * vendor->numberOfSoldItems);
			memset(vendor->soldItemList, 0x00, sizeof(vendorItemEntry_t) * vendor->numberOfSoldItems);
			for(sint32 i=0; i<vendorData->numberOfItems; i++)
			{
				vendor->soldItemList[i].itemTemplateId = vendorData->itemList[i].itemTemplateId;
				vendor->soldItemList[i].stacksize = vendorData->itemList[i].stacksize;
				vendor->soldItemList[i].itemInstance = NULL; // we create the item on demand (on vendor 'use')
				vendor->soldItemList[i].sequence = vendorData->itemList[i].sequence;
			}
		}
		else
		{
			vendor->numberOfSoldItems = 0;
			vendor->soldItemList = NULL;
		}
	}
	vendorLoadState = 1;
}

// synchronous loading of vendor data from the db
void vendor_init()
{
	vendorLoadState = 0;
	DataInterface_Vendor_getVendorList(_cb_vendor_init, NULL);
	while( vendorLoadState == 0 ) Sleep(100);
}