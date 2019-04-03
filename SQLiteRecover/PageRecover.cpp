#include "stdafx.h"


#include "SQLiteRecovery.h"



VOID SQLiteRecovery::PageTravelRecovery(){


	GetPageListFromSQLite();

	for (int i = 0; i < pages.size(); i++){


		// 1. ���������鼭 �÷���Ȯ��
		// 2. 0d �̸� GetSchemaFromNormalRecord
		// 3. �ٸ��Ÿ� ���ã��
		INT data;

		
		switch (pages[i].Page[0])
		{
		case INTERNAL_PAGE:


			break;

		case LEAF_PAGE:
		{
			 data = GetSchemaFromPageNumber(i);
			  if (data == -1){

				  /*
				  
					1. ��Ű�� Ž��
					2. ã�ƾ���



					1. ���� �� ������ ����
					2. ��Ű�� Ž�� �Ǵ� ��Ű�� �����Ȱŷ� �������ڵ� ��ġ�ϴ� ���� Ž��(�� ������ ����)
					3. �����µ� ����
					4. GetNormalRecord ó�� ���� �� �������� �����ϰ� ����.
				  
				  */

				  


			  }
			  else{
				  GetDeletedRecord(pages[i], &tables[data]);
			  }
			  break;
		}
			

		default:
			break;
		}

	}



}

VOID SQLiteRecovery::GetPageListFromSQLite(){

	INT SplitStart = 0;
	INT PageNumber = 1;
	while (SplitStart < FileSize){

		SQLitePage page;
		page.PageNumber = PageNumber;
		page.Page = new BYTE[PageSize + 1];
		memcpy(page.Page, &SQLITE_BUFFER[SplitStart], PageSize);

		pages.push_back(page);

		SplitStart += PageSize;
		PageNumber++;

	}

}


INT SQLiteRecovery::GetSchemaFromByteScan(SQLitePage Page){

	INT Data = -1;
	for (int i = 0; i < tables.size(); i++){

		/*

			1. ó�� 0��°���� 00���ִ°����� ã��
			2. 00���ִ°����� �������ִ� �������� ã�� 
			3. �ű⼭���� ����Ʈ��ĵ�ϸ��.

		*/

		//Data 

	}

	return Data;
}

INT SQLiteRecovery::GetSchemaFromPageNumber(INT PageNumber){

	
	for (int i = 0; i < tables.size(); i++){

		//if (tables[i].RootPageNum == )

		for (int k = 0; k < tables[i].LeafPageNum.size(); k++){


			if (PageNumber == tables[i].LeafPageNum[k])
				return i; // ������ ��Ű��~!

		}


	}

	return -1;		// ��Ű�� ����Ʈ ��ĵ���� ã�ƾ���
}

INT SQLiteRecovery::GetSchemaFromNormalRecord(SQLitePage Page) // Get Table index
{

	BYTE NormalCell[2];
	memcpy(NormalCell, &Page.Page[8], 2);
	INT celloffset = ut.LongIntToByte(NormalCell, 2);
	



	if (celloffset == 0)
		return -1;



	/*
	1. ��� ��� ������
	2. row id ������
	3. �������������! (���� ���ϰ�)
	4. ���̸�ŭ ��� ����(��� ������)
	5. Ÿ�� ���̰� ��ġ�Ѵٸ� ?
	6. Ÿ�Գ��� �� �ؼ� ���̺� �ٰ�

	*/
	INT StartOffset = celloffset;
	INT EndOffset = StartOffset;

	/*
		���ڵ������, ROW ID, 
	*/

	for (int k = 0; k < 2; k++){

		INT SizeOfLengthField = GetAvailableByteByBitPattern(Page.Page, StartOffset, 1);
		//		HeaderSize += SizeOfLengthField;

		if (k == 0){
			BYTE* SizeOfRecord = new BYTE[SizeOfLengthField + 1];
			memcpy(SizeOfRecord, &Page.Page[StartOffset], SizeOfLengthField);
			EndOffset += GetLengthOfBitPattern(SizeOfRecord, SizeOfLengthField);
			EndOffset += SizeOfLengthField;
			delete[] SizeOfRecord;
		}

		StartOffset += SizeOfLengthField;
	}

	// HeaderSize
	INT SizeOfLengthField = GetAvailableByteByBitPattern(Page.Page, StartOffset, 1);
	BYTE* SizeOfRecord = new BYTE[SizeOfLengthField + 1];
	memcpy(SizeOfRecord, &Page.Page[StartOffset], SizeOfLengthField);

	INT HeaderSize = GetLengthOfBitPattern(SizeOfRecord, SizeOfLengthField);
	StartOffset += SizeOfLengthField;
	delete[] SizeOfRecord;

	INT NumberOfColumn = 0;
	std::vector<SQLITE_COLUMN>	TargetColumn;
	INT EndOfHeader = StartOffset + HeaderSize - 1;

	
	while (StartOffset < EndOfHeader){

		INT SizeOfLengthField = GetAvailableByteByBitPattern(Page.Page, StartOffset, 1);
		BYTE* length = new BYTE[SizeOfLengthField + 1];
		memcpy(length, &Page.Page[StartOffset], SizeOfLengthField);
		INT ColumnByte = GetLengthOfBitPattern(length, SizeOfLengthField);

		SQLITE_COLUMN column;
		if (1 <= ColumnByte && ColumnByte <= 9){

			column.Type = L"INTEGER";
		}
		else if( ColumnByte == 0){
			column.Type = L"NULL";
		}
		else if (12 <= ColumnByte && ColumnByte % 2 == 0){
			column.Type = L"BLOB";
		}
		else{
			column.Type = L"TEXT";
		}


		TargetColumn.push_back(column);
		StartOffset += SizeOfLengthField;
		delete[] length;

	}


	for (int i = 0; i < tables.size(); i++){

		//��Ű�� �񱳱���
		int k = 0;
		if (tables[i].NumberOfField == TargetColumn.size()){


			/*for ( k = 0; k < TargetColumn.size(); k++){

				if (TargetColumn[k].Type == L"NULL"){
					continue;
				}
				else if (TargetColumn[k].Type != tables[i].Column[k].Type){
					break;
				}

			}*/

			return i;

		}
		// �ǵڿ� ī�� �÷� �ϳ��߰���
// 		else if (tables[i].NumberOfField == TargetColumn.size() + 1){
// 
// 			return i;
// 		}

/*
		if (k == tables[i].NumberOfField)
			return i;*/
		

	}
	return -1;
}