#ifndef HEADER_H
#define HEADER_H

typedef enum
{
    eConnTypeUnknown,
    eConnType3Point5mm,
    eConnTypeQuarter,
    eConnTypeAtapiInternal,
    eConnTypeRCA,
    eConnTypeOptical,
    eConnTypeOtherDigital,
    eConnTypeOtherAnalog,
    eConnTypeMultichannelAnalogDIN,
    eConnTypeXlrProfessional,
    eConnTypeRJ11Modem,
    eConnTypeCombination
} EPcxConnectionType;

typedef enum
{
    eGeoLocRear = 0x1,
    eGeoLocFront,
    eGeoLocLeft,
    eGeoLocRight,
    eGeoLocTop,
    eGeoLocBottom,
    eGeoLocRearPanel,
    eGeoLocRiser,
    eGeoLocInsideMobileLid,
    eGeoLocDrivebay,
    eGeoLocHDMI,
    eGeoLocOutsideMobileLid,
    eGeoLocATAPI,
    eGeoLocNotApplicable,
    eGeoLocReserved6,
    EPcxGeoLocation_enum_count
} EPcxGeoLocation;

typedef enum
{
    eGenLocPrimaryBox = 0,
    eGenLocInternal,
    eGenLocSeparate,
    eGenLocOther,
    EPcxGenLocation_enum_count
} EPcxGenLocation;

typedef enum
{
    ePortConnJack = 0,
    ePortConnIntegratedDevice,
    ePortConnBothIntegratedAndJack,
    ePortConnUnknown
} EPxcPortConnection;

typedef struct
{
    DWORD ChannelMapping,
    Color;
    EPcxConnectionType ConnectionType;
    EPcxGeoLocation GeoLocation;
    EPcxGenLocation GenLocation;
    EPxcPortConnection PortConnection;
    BOOL IsConnected;
} KSJACK_DESCRIPTION, *PKSJACK_DESCRIPTION;

typedef struct _tagKSJACK_DESCRIPTION2
{
    DWORD DeviceStateInfo,
    JackCapabilities;
} KSJACK_DESCRIPTION2, *PKSJACK_DESCRIPTION2;

typedef enum
{
    KSJACK_SINK_CONNECTIONTYPE_HDMI = 0,
    KSJACK_SINK_CONNECTIONTYPE_DISPLAYPORT,
} KSJACK_SINK_CONNECTIONTYPE;

#define MAX_SINK_DESCRIPTION_NAME_LENGTH 32
typedef struct _tagKSJACK_SINK_INFORMATION
{
    KSJACK_SINK_CONNECTIONTYPE ConnType;
    WORD ManufacturerId,
    ProductId,
    AudioLatency;
    BOOL HDCPCapable,
    AICapable;
    UCHAR SinkDescriptionLength;
    WCHAR SinkDescription[MAX_SINK_DESCRIPTION_NAME_LENGTH];
    LUID PortId;
} KSJACK_SINK_INFORMATION, *PKSJACK_SINK_INFORMATION;

#endif // HEADER_H
