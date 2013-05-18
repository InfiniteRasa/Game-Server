
typedef struct  
{
	uint32 itemTemplateId;
	uint32 stacksize;
	sint32 sequence;
	item_t* itemInstance; // physical entity that is used to display the item
}vendorItemEntry_t;

typedef struct _vendorData_t 
{
	sint32 vendorPackageId;
	// list of sold items
	sint32 numberOfSoldItems;
	vendorItemEntry_t* soldItemList;
}vendorData_t;

void vendor_init();