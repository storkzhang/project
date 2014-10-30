#include "GetPhysicalSerialNumber.h"
void GetPhysicalSerialNumber::GetDiskPhysicalSN(char pchDiskPhysicalSN[14])
{
    BYTE IdOutCmd[530];
    HANDLE drive=CreateFile(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL );
    if ( drive == INVALID_HANDLE_VALUE )
    {
        pchDiskPhysicalSN[ 0 ] = 0;
        return ;
    }
    GETVERSIONOUTPARAMS VersionParams;
    DWORD cbBytesReturned = 0;
    memset( ( void* ) &VersionParams, 0, sizeof( VersionParams ) );
    if ( ! DeviceIoControl( drive, DFP_GET_VERSION, NULL, 0, &VersionParams, sizeof( VersionParams ), &cbBytesReturned, NULL ) )
    {
        pchDiskPhysicalSN[ 0 ] = 0;
        return ;
    }
    if (VersionParams.bIDEDeviceMap<=0)
    {
        pchDiskPhysicalSN[ 0 ] = 0;
        return ;
    }
    BYTE bIDCmd = 0;
    SENDCMDINPARAMS scip;
    bIDCmd = ( VersionParams.bIDEDeviceMap >> 0 & 0x10 ) ? IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;
    memset( &scip, 0, sizeof( scip) );
    memset( IdOutCmd, 0, sizeof( IdOutCmd ) );
    scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
    scip.irDriveRegs.bFeaturesReg = 0;
    scip.irDriveRegs.bSectorCountReg = 1;
    scip.irDriveRegs.bSectorNumberReg = 1;
    scip.irDriveRegs.bCylLowReg = 0;
    scip.irDriveRegs.bCylHighReg = 0;
    scip.irDriveRegs.bDriveHeadReg= 0xA0 | ( ( ( BYTE ) drive & 1 ) << 4 );
    scip.irDriveRegs.bCommandReg = bIDCmd;
    scip.bDriveNumber = ( BYTE ) drive;
    scip.cBufferSize = IDENTIFY_BUFFER_SIZE;
    if ( ! DeviceIoControl( drive, DFP_RECEIVE_DRIVE_DATA, &scip, sizeof( SENDCMDINPARAMS)- 1, ( LPVOID )&IdOutCmd, sizeof( SENDCMDOUTPARAMS ) + IDENTIFY_BUFFER_SIZE - 1, &cbBytesReturned, NULL ) )
    {
        pchDiskPhysicalSN[ 0 ] = 0;
        return ;
    }
    USHORT *pIdSector = ( USHORT * )( ( PSENDCMDOUTPARAMS )IdOutCmd )->bBuffer;
    int nPosition = 0;
    for( int nIndex = 13; nIndex < 20; nIndex++ )
    {
        pchDiskPhysicalSN[ nPosition++ ]=( unsigned char )( pIdSector[ nIndex ] / 256 );
        pchDiskPhysicalSN[ nPosition++ ]=( unsigned char )( pIdSector[ nIndex ] % 256 );
    }
}

void GetPhysicalSerialNumber::GetDiskPhysicalSerialNumber( char* pchSerialNumber )
{
    char pchDiskPhysicalSN[ 14 ];
    GetDiskPhysicalSN( pchDiskPhysicalSN );
    int nSerial[ 14 ];
    for( int i = 0 ; i < 14; ++ i )
    {
        nSerial[ i ] = pchDiskPhysicalSN[ i ];
        nSerial[ i ] &= 0x000000ff;
    }
    sprintf( pchSerialNumber, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
                               nSerial[ 0 ],
                               nSerial[ 1 ],
                               nSerial[ 2 ],
                               nSerial[ 3 ],
                               nSerial[ 4 ],
                               nSerial[ 5 ],
                               nSerial[ 6 ],
                               nSerial[ 7 ],
                               nSerial[ 8 ],
                               nSerial[ 9 ],
                               nSerial[ 10 ],
                               nSerial[ 11 ],
                               nSerial[ 12 ],
                               nSerial[ 13 ]); // Should use scl::FormatString inside 
}