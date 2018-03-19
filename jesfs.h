/*******************************************************************************
* JesFs.h: Header Files for JesFs
*
* JesFs - Jo's Embedded Serial File System
* Tested on Win and TI-RTOS CC1310 Launchpad
*
* (C) joembedded@gmail.com 2018
* Please regard: This is Copyrighted Software!
* It may be used for education or non-commercial use, but without any warranty!
* For commercial use, please read the included docu and the file 'license.txt'
*******************************************************************************/

/* List of Errors
-100: SPI Init (Hardware)
-101: Flash Timeout WaitBusy
-102:  SPI Can not set WriteEnableBit (Flash locked?)
-103: ID:Unknown/illegal Flash Density (described the size)
-104: ID:Unknown Flash ID (eg. 0xC228 for Macronix M25xx, see docu)
-105: Illegal flash addr
-106: Block crosses sector border
-107: fs_start found problems in the filesystem structure (-> run recover)
-108: Unknown MAGIC, this Flash is either unformated or contains other data
-109: Flash-ID in the Flash Index does not match Hardware-ID (-> run recover)
-110: Filename to long/short
-111: Too many files, Index full! (ca. 1000 for 4k sectors)
-112: Sector border violated (before write)
-113: Flash full! No free sectors available or Flash not formatted
-114: Index corrupted (-> run recover)
-115: Number out of range Index (fs_stat)
-116: No active file at this entry (fs_stat)
-117: Illegal descriptor or file not open
-118: File not open for writing
-119: Index out of range
-120: Illegal sector address
-121: Short circle in sector list (-> run recover)
-122: sector list contains illegal file owner (-> run recover)
-123: Illegal sector type (-> run recover)
-124: File not found
-125: Illegal file flags (e.g. trying to delet a file opened for write)
-126: Illegal file system structure (-> run recover)
-127: Closed files can not be continued (for writing)
-128: Sector defect ('Header with owner') (-> run recover)
-129: File descriptor corrupted.
-130: Try to write to (unclosed) file in RAW with unknown end position
-131: Sector corrupted: Empty marked sector not empty
-132: File is empty
 */

#ifdef __cplusplus
extern "C"{
#endif

//------------------- Area for User Settings START -----------------------------
/* SF_xx_TRANSFER_LIMIT:
* If defined, Read- and Write-Transfers are chunked to this maximum Limit
* for normal operation set to maximum or undefine ;-)
* Recommended for Read to CPU: >=64, Best: undefine
* for Write to SPI: Because standard SPI has 256-Byte pages, chungs are
* already small. Even, feel free to set to somethig smaller >=64
*
* Maybe with TI-RTOS and the X110-Emulator on a CC1310 the heap does not like
* too big chunks. However, it makes transfers slower.
*/
//#define SF_RD_TRANSFER_LIMIT 64
//#define SF_TX_TRANSFER_LIMIT 64

// Sample-Flash ID MACRONIX (Ultra-Low-Power), add others
#define MACRONIX_MANU_TYP    0xC228  // Macronix MX25R-Low-Power-Series first 2 ID-Bytes (without Density)

//------------------- Area for User Settings END -------------------------------


#define FNAMELEN 25  // maximum filename len (Byte 26 must be 0, as in regular strings)...

// Startflags (fs_start())
#define FS_START_NORMAL   0 // ca. 20 msec per MB on an empty Flash
#define FS_START_FAST     1  // ca. 10 msec per MB on an empty Flash, but less checks
//#define FS_START_PEDANTIC   2 // Reserved for Version >V1.x
#define FS_START_RESTART  128 // ca. 50 usec if Flash data is already known. Else FS_START_NORMAL

// Flags for (fs_open) files
#define SF_OPEN_READ      1 // open for read only
#define SF_OPEN_CREATE    2 // create file in any case
#define SF_OPEN_WRITE     4 // open for writing
#define SF_OPEN_RAW       8 // just open
#define SF_OPEN_CRC    	  16 // if set: calculate CRC32 for file while reading/writing
#define SF_OPEN_ESC_MODE  32 // *** Reserved for >V1.x, see Docu ***
// The following Flags are not relevant for the Filesystem, but for external cccess
#define SF_OPEN_EXT_SYNC   64 // File should be synced to external filesystem
#define SF_OPEN_EXT_HIDDEN 128 // File is normally NOT accessible from outside (e.g. for KeyStore-Files, etc..)

// Flags for Statistic
#define FS_STAT_ACTIVE 1
#define FS_STAT_INACTIVE 2
#define FS_STAT_UNCLOSED 4

// Filedescriptor
typedef struct{
	uint32_t    _head_sadr;   // Hidden, head of file
	uint32_t    _wrk_sadr; // Hidden, working
	uint32_t    file_pos; // end pos is the current file len
	uint32_t    file_len;  // len after open (set by fs_open)
	uint32_t    file_crc32; // running CRC32 according ISO 3309, FFFFFFFF if not used (only with SF_OPEN_CRC)

	uint16_t    _sadr_rel;   // Hidden, relative
	uint8_t     open_flags;  // current file flags (set by fs_open)
} FS_DESC;

// Statistic descriptor
typedef struct{
	char fname[FNAMELEN+1]; // Max. filenam len V1.0 (25+0x00)
	uint32_t file_len;
	uint32_t file_crc32; // CRC32 in Flash for this file, according ISO 3309, FFFFFFFF if not used (only with SF_OPEN_CRC)
	uint32_t _head_sadr;   // Hidden, head of file
	uint8_t  disk_flags;    // file flags on disk�(written by fs_close)
} FS_STAT;

//-------------------- HighLevel Functions --------------------------
int16_t fs_start(uint8_t mode);
void fs_deepsleep(void);
int16_t fs_format(uint32_t f_id);
int32_t fs_read(FS_DESC *pdesc, uint8_t *pdest, uint32_t anz);
int16_t fs_rewind(FS_DESC *pdesc);
int16_t fs_open(FS_DESC *pdesc, char* pname, uint8_t flags);
int16_t fs_write(FS_DESC *pdesc, uint8_t *pdata, uint32_t len);
int16_t fs_close(FS_DESC *pdesc);
int16_t fs_delete(FS_DESC *pdesc);
int16_t fs_info(FS_STAT *pstat, uint16_t fno );
uint32_t fs_get_crc32(FS_DESC *pdesc);

#ifdef __cplusplus
}
#endif
//End