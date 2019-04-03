#include "stdafx.h"
#include "SQLiteReader.h"


SQLReader::SQLReader(TCHAR* pszfilename){


	CFile f;
	HANDLE hFile = CreateFile(pszfilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	file_size = GetFileSize(hFile, NULL);

	CloseHandle(hFile);
	if (!f.Open(pszfilename, CFile::modeRead))
	{
		TRACE(_T("File could not be opened %d\n"));
		exit(1);

	}

	SQLBUFFER = (BYTE*)malloc(file_size);
	f.Read(SQLBUFFER, file_size);
	f.Close();
}


SQLReader::SQLReader(TCHAR* pszfilename, BOOL OverFlow){
	CFile f;
	HANDLE hFile = CreateFile(pszfilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	file_size = GetFileSize(hFile, NULL);

	CloseHandle(hFile);
	if (!f.Open(pszfilename, CFile::modeRead))
	{
		TRACE(_T("File could not be opened %d\n"));
		exit(1);

	}
	

	SQLBUFFER = (BYTE*)malloc(file_size);
	f.Read(SQLBUFFER, file_size);
	f.Close();
}

INT SQLReader::GetSplitSQLiteByPageSize(){

	INT SplitStart =0;
	//CFile f;
	
	INT PAGE_NUM = 0;

	while (SplitStart < file_size){
	//	CString name;
	//	name.Format(L"SplitPageNumber_%d", PAGE_NUM);
	//	f.Open(name, CFile::modeCreate | CFile::modeReadWrite);
		//BYTE* temp = (BYTE*)malloc(PageSize);
		//memcpy(temp, &SQLBUFFER[SplitStart], PageSize);

		SplitSQLiteContainer sqlite_container;
		sqlite_container.PAGE = new BYTE[PageSize];
		memcpy(sqlite_container.PAGE, &SQLBUFFER[SplitStart], PageSize);
		sqlite_container.PageFlag = SQLBUFFER[SplitStart];
		memcpy(sqlite_container.NextFreeSpace, &SQLBUFFER[SplitStart + 1], 2);
		memcpy(sqlite_container.NumberOfRecord, &SQLBUFFER[SplitStart + 3], 2);
		memcpy(sqlite_container.OffsetOfTheFirstBytesOfRecord, &SQLBUFFER[SplitStart + 5], 2);
		sqlite_container.NumberOfFragmentedFreeByte = SQLBUFFER[SplitStart + 7];
		memcpy(sqlite_container.ChildInternalPage, &SQLBUFFER[SplitStart + 8], 4);
		sqlite_container.PAGE_NUMBER = PAGE_NUM;
		splitpage.push_back(sqlite_container);
	//	f.Write(&SQLBUFFER[SplitStart], PageSize);
		//free(temp);
		SplitStart += PageSize;
		
		
		PAGE_NUM++;
		
	//	f.Close();

	}

	return PAGE_NUM;

}


VOID SQLReader::SetPageSize(INT _PageSize){
	PageSize = _PageSize;
}
BOOL SQLReader::SaveStringToCSV(SplitSQLiteContainer Page, INT END){

	if ((ut.IntToByte(Page.NumberOfRecord, 2) == 1 && Page.PAGE_NUMBER != 2) || END == 4){
		

		// 1. �ϴ� ���ڵ� ��ü ���̸� ���ϰ�
		// 2. �ű� ����Ʈ�ȿ��� ���ϸ� �����Ⱚ�� ���� ������.

		INT end = END;
		INT start = end;

		INT RecordLength;

		BSTR    bstrWide;
		INT     nLength, i;
// 		
// 		CString CSV_NAME;
// 		CSV_NAME.Format(L"Contents_PageNumber.csv", Page.PAGE_NUMBER);
// 		std::string fileName = std::string(CT2CA(CSV_NAME.operator LPCWSTR()));
		
		FILE* csv = fopen("UnknownPage.csv", "a+");
		while (end < PageSize){
			if (0x00 <= Page.PAGE[end] && Page.PAGE[end] <= 0x1E){
			//	if (Page.PAGE[end] == 0x00 || Page.PAGE[end] == 0x1E || Page.PAGE[end] == 0x06 || Page.PAGE[end] == 0x0C || Page.PAGE[end] == 0x04){
				if (end - start < 9){
					start = end + 1;
					end++;
					continue;
				}
				CHAR* str = new CHAR[end - start + 1];
				memcpy(str, &Page.PAGE[start], end - start + 1);
				str[end - start] = NULL;

				CHAR*   pszAnsi;

				nLength = MultiByteToWideChar(CP_UTF8, 0, str, end - start + 1, NULL, NULL);
				bstrWide = SysAllocStringLen(NULL, nLength);

				MultiByteToWideChar(CP_UTF8, 0, str, end - start + 1, bstrWide, nLength);

				nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
				pszAnsi = new CHAR[nLength];

				WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
				SysFreeString(bstrWide);

				//pszAnsi;
				fprintf(csv, "%s\n", pszAnsi);
				//CString temp(pszAnsi);
			//	wprintf(L"%s\n",temp);
				delete[] pszAnsi;
				delete[] str;
				start = end + 1;
				end++;

			}
			else{
				end++;
			}

		}

		fclose(csv);
	}


	return TRUE;
}

BOOL SQLReader::ExtractStringData(SplitSQLiteContainer Page){


	switch (Page.PageFlag)
	{
	case LEAF:

		// 1. ���ڵ� ���� ���
		// 2. �÷� 1���� ���
	//	SaveStringToCSV(Page, ut.IntToByte(Page.OffsetOfTheFirstBytesOfRecord, 2) + 8);

		return TRUE;
		
	case INTERNAL:
		return FALSE;

	default:
		SaveStringToCSV(Page, 4);
		return FALSE;
	}


	

}

BOOL SQLReader::SetOverFlowSQLite(){

/*
	INT SplitStart = 0;
	memcpy(overflow.PAGE, &SQLBUFFER[SplitStart], PageSize * 2);
	overflow.PageFlag = SQLBUFFER[SplitStart];
	memcpy(overflow.NextFreeSpace, &SQLBUFFER[SplitStart + 1], 2);
	memcpy(overflow.NumberOfRecord, &SQLBUFFER[SplitStart + 3], 2);
	memcpy(overflow.OffsetOfTheFirstBytesOfRecord, &SQLBUFFER[SplitStart + 5], 2);
	overflow.NumberOfFragmentedFreeByte = SQLBUFFER[SplitStart + 7];
	memcpy(overflow.ChildInternalPage, &SQLBUFFER[SplitStart + 8], 4);
*/

	return TRUE;
}

BOOL SQLReader::Run(BOOL){

	CurrentOffset = ut.IntToByte(overflow.OffsetOfTheFirstBytesOfRecord, 2);
	int RecordSize = GetRecordSize(CurrentOffset);


	return TRUE;
}


INT SQLReader::GetRecordSize(INT OFFSET_TO_SIZE_AREA){

	int size = 1;
	int startSizeOffset = OFFSET_TO_SIZE_AREA;
	while (SQLBUFFER[OFFSET_TO_SIZE_AREA] & 0x80)
	{
		size++;
		OFFSET_TO_SIZE_AREA++;
	}




	return size;

}

VOID SQLReader::Run(){

	for (int i = 0; i < splitpage.size(); i++){
		
		if (ExtractStringData(splitpage[i])){

		}


		/*
		
		1. �ϴ� NumberOfRecord�� NextFreeSpace �� üũ�ϰ�
		2. ù ��° ���ڵ� �������� üũ�Ѵ�. 
		3. ��¥�� �����ǵ� ���ڵ�� Size���� Ÿ���� ������������. 0 : NULL, N(1-4) : Int , 7 : IEEE Float, 8-11 : Reserved, N > 12 BLOB, n > 13 : TEXT

		*/

		

	}

}

BYTE SQLReader::GetPageFlag(INT START_PAGE){


	return SQLBUFFER[START_PAGE];
}