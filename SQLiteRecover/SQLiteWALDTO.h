#include "stdafx.h"
#ifndef __SQLITEWALDTO_H__
#define	__SQLITEWALDTO_H__

#define		WAL_HEADER_SIZE				0x20
#define		WAL_FRAME_HEADER_SIZE		0x18

struct SQLiteWALHeader{
	BYTE FileSignature[4];
	BYTE FileFormatVersion[4];

	BYTE PageSize[4]; // �߿�

	BYTE CheckPointSequenceNumber[4];
	BYTE SALT1[4];
	BYTE SALT2[4];

	BYTE ChecksumPart1[4];
	BYTE ChecksumPart2[4];
};
struct mWALFrameHeader {

	BYTE SQLitePageNumber[4];				// ��Ű�� ã����
	BYTE CommitRecord[4];
	BYTE SALT1_COPY[4];	//WAL Header�� ��ġ�ϴ��� üũ
	BYTE SALT2_COPY[4];	//WAL Header�� ��ġ�ϴ��� üũ
	BYTE Checksum1[4];
	BYTE Checksum2[4];

};

struct WALFrame {
	mWALFrameHeader header;
	BYTE* Page;
	//std::list<WALFrame> next;
};


#endif