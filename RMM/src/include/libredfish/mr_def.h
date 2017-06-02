#ifndef __MR_DEF__
#define __MR_DEF__


enum MessageType {
	MSGStatusChange = 0,			/* Resource Status Change*/
	MSGResourceUpdated,			/* Resource Update*/
	MSGResourceAdded,			/* Resource Add*/
	MSGResourceRemoved,			/* Resource Remove*/
	MSGAlert,				/* Resource Alert*/
	MSGRackStatusChange,			/* Rack Status Change*/
	MSGRackResourceUpdated,			/* Rack Info Update*/
	MSGPSUStatusChange,			/* PSU Status Change*/
	MSGPSUResourceUpdated,			/* PSU Info Update*/
	MSGPSUResourceAdded,			/* PSU Add*/
	MSGPSUResourceRemoved,			/* PSU Remove*/
	MSGFanResourceAdded,			/* FAN Add*/
	MSGFanResourceRemoved,			/* FAN Remove*/
	MSGFanStatusChange,			/* FAN Status CHange*/
	MSGMBPStatusChange,			/* MBP Status Change*/
	MSGMBPResourceUpdated,			/* MBP Info update*/
	MSGMBPResourceAdded,			/* MBP Add*/
	MSGMBPResourceRemoved,			/* MBP Remove*/
	MSGDrawerStatusChange,			/* Drawer Status Change*/
	MSGDrawerResourceUpdated,		/* Drawer update*/
	MSGDrawerResourceAdded,			/* Drawer Add*/
	MSGDrawerResourceRemoved,		/* Drawer Remove*/
	MSGDrawerAlert,				/* Drawer Alert*/
	MSGPZoneResourceUpdated,		/* Power Zone Info update*/
	MSGPZoneResourceAdded,			/* Power Zone Add*/
	MSGPZoneResourceRemoved,		/* Power Zone Remove*/
	MSGTZoneResourceUpdated,		/* Thermal Zone Info update*/
	MSGTZoneResourceAdded,			/* Thermal Zone Add*/
	MSGTZoneResourceRemoved			/* Thermal Zone Remove*/
};

#define RF_PRE_FIX_INFO		"RMM"
#define RF_PRE_VER_INFO		"1.0.0"

#endif
