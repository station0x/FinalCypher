// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "AdMobHelper.h"

FAdMobHelper::FInitAdRequestMemoryHandler::FInitAdRequestMemoryHandler(const FAdMobAdRequest& InReq)
	: Req(InReq)
{
	FMemory::Memzero(&RawReq, sizeof(firebase::admob::AdRequest));

	RawReq.birthday_day			= InReq.BirthdayDay;
	RawReq.birthday_month		= InReq.BirthdayMonth;
	RawReq.birthday_year		= InReq.BirthdayYear;
	RawReq.extras_count			= InReq.Extras.Num();
	RawReq.keyword_count		= InReq.Keywords.Num();
	RawReq.test_device_id_count	= InReq.TestDeviceIds.Num();
	RawReq.gender				= static_cast<firebase::admob::Gender>(InReq.Gender);

	if (RawReq.test_device_id_count > 0)
	{
		RawReq.test_device_ids = (const char**)FMemory::Malloc(RawReq.test_device_id_count * sizeof(const char*));

		for (uint32 i = 0; i < RawReq.test_device_id_count; ++i)
		{
			const FString& DeviceId = Req.TestDeviceIds[i];
			const int32 DeviceIdByteSize = DeviceId.Len() * sizeof(char);

			char* DeviceIdStr = (char*)FMemory::Malloc(DeviceIdByteSize + 1);

			FCStringAnsi::Strcpy(DeviceIdStr, DeviceIdByteSize, (const char*)TCHAR_TO_UTF8(*DeviceId));

			DeviceIdStr[DeviceIdByteSize] = '\0';

			RawReq.test_device_ids[i] = DeviceIdStr;
		}
	}

	if (RawReq.keyword_count > 0)
	{
		RawReq.keywords = (const char**)FMemory::Malloc(RawReq.keyword_count * sizeof(const char*));
		Keywords.Reserve(RawReq.keyword_count);

		for (uint32 i = 0; i < RawReq.keyword_count; ++i)
		{		
			RawReq.keywords[i] = Keywords.Emplace_GetRef(TCHAR_TO_UTF8(*Req.Keywords[i])).c_str();
		}
	}

	if (RawReq.extras_count > 0) 
	{
		RawReq.extras = (const firebase::admob::KeyValuePair*) FMemory::Malloc(RawReq.extras_count * sizeof(const firebase::admob::KeyValuePair));
		int32 i = 0;
		for (const TPair<FString, FString>& Extra : Req.Extras)
		{
			const int32 KeyByteSize = Extra.Key  .Len() * sizeof(ANSICHAR);
			const int32 ValByteSize = Extra.Value.Len() * sizeof(ANSICHAR);

			char* Key = (char*)FMemory::Malloc(KeyByteSize + 1);
			char* Val = (char*)FMemory::Malloc(ValByteSize + 1);
				
			FCStringAnsi::Strcpy(Key, KeyByteSize, (const ANSICHAR*)TCHAR_TO_UTF8(*Extra.Key));
			FCStringAnsi::Strcpy(Val, ValByteSize, (const ANSICHAR*)TCHAR_TO_UTF8(*Extra.Value));

			Key[KeyByteSize] = '\0';
			Val[ValByteSize] = '\0';

			const_cast<firebase::admob::KeyValuePair*>(&RawReq.extras[i])->key   = Key; 
			const_cast<firebase::admob::KeyValuePair*>(&RawReq.extras[i])->value = Val;			

			++i;
		}
	}
}

FAdMobHelper::FInitAdRequestMemoryHandler::~FInitAdRequestMemoryHandler()
{
	for (uint32 i = 0; i < RawReq.test_device_id_count; ++i)
	{
		FMemory::Free((void*)RawReq.test_device_ids[i]);
	}
	FMemory::Free(RawReq.test_device_ids);
	
	if (RawReq.keyword_count > 0)
	{
		FMemory::Free(RawReq.keywords);
	}

	if (RawReq.extras_count > 0)
	{
		for (uint32 i = 0; i < RawReq.extras_count; ++i)
		{
			FMemory::Free((void*)RawReq.extras[i].value);
			FMemory::Free((void*)RawReq.extras[i].key);
		}
		FMemory::Free((void*)RawReq.extras);
	}
}

const firebase::admob::AdRequest& FAdMobHelper::FInitAdRequestMemoryHandler::Get()
{
	return RawReq;
}

firebase::admob::AdParent FAdMobHelper::GetAdParent()
{
#if PLATFORM_ANDROID
	return static_cast<firebase::admob::AdParent>(FJavaWrapper::GameActivityThis);
#elif PLATFORM_IOS
	FIOSView* View = [IOSAppDelegate GetDelegate].IOSView;
	return static_cast<firebase::admob::AdParent>(View);
#else
	return nullptr;
#endif
}

