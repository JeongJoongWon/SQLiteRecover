#include "stdafx.h"
#include <vector>
#include "UTIL.h"
#ifndef __JANG_H__
#define __JANG_H__


#define LEAF		0x0D
#define INTERNAL	0x05
#define OVERFLOW	0x00


struct SplitSQLiteContainer{

	INT		PAGE_NUMBER;
	BYTE	PageFlag;
	BYTE	NextFreeSpace[2];
	BYTE	NumberOfRecord[2];
	BYTE	OffsetOfTheFirstBytesOfRecord[2];
	BYTE	NumberOfFragmentedFreeByte;
	BYTE	ChildInternalPage[4];

	BYTE*	PAGE;

};


struct SQLiteOverFlowPage
{
	BYTE	PageFlag;
	BYTE	NextFreeSpace[2];
	BYTE	NumberOfRecord[2];
	BYTE	OffsetOfTheFirstBytesOfRecord[2];
	BYTE	NumberOfFragmentedFreeByte;
	BYTE	ChildInternalPage[4];

//	BYTE	PAGE[PageSize * 2];			//�ϴ� 2���ε�('�ֽ�' Ű���尡 �ִ°���) ������ ����ؼ� ���߿� �ٲ����.
};



/*
	���� �ؾߵɰŴ� : 
			1. Spit�Ⱦֵ��߿� overflow���ֵ� ã�ƾ��ϰ�,
			2. Split�Ⱦֵ� �����ȵǴ¾ֵ� ã�Ƽ� ��� ���� ��⸸�������ϰ�
			3. �ϴ� �����÷ο���� ����
*/

class SQLReader
{
private:

	BYTE* SQLBUFFER;
	int file_size;


	std::vector<SplitSQLiteContainer>	splitpage;


	//�ӽ�
	SQLiteOverFlowPage overflow;
	INT CurrentOffset;
	UTIL ut;


	INT PageSize;

public:
	SQLReader(TCHAR* pszfilename);
	SQLReader(TCHAR* pszfilename, BOOL OverFlow);
	INT GetSplitSQLiteByPageSize();


	VOID SetPageSize(INT _PageSize);

	/*���⼭��*/
	VOID Run();

	

	/*�̰�*/
	BOOL ExtractStringData(SplitSQLiteContainer Page);
	BOOL SaveStringToCSV(SplitSQLiteContainer Page, INT END);


	
	
	BOOL Run(BOOL);

	/*�Ƚ�*/
	INT GetRecordSize(INT OFFSET_TO_SIZE_AREA); // �̰� ��������ݴ�.. �׳� �����¾ƴѰ�? �ϴܳ��־���
	/*�Ƚ�*/
	BYTE GetPageFlag(INT START_PAGE);//�ӽ�
	BOOL SetOverFlowSQLite();
	

	/*			if Deleted Cell Exists			*/		

};

#endif