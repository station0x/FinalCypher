// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Storage/StorageReference.h"

#include "FirebaseFeatures.h"

#include "Async/Async.h"

#if WITH_FIREBASE_STORAGE
THIRD_PARTY_INCLUDES_START
#	include "firebase/app.h"
#	include "firebase/storage.h"
#	include "firebase/storage/controller.h"
#	include "firebase/storage/listener.h"
THIRD_PARTY_INCLUDES_END
#endif

#if WITH_FIREBASE_STORAGE
void FFirebaseStorageMetadata::CreateInternal() const
{
	if (!Metadata)
	{
		const_cast<FFirebaseStorageMetadata&>(*this).Metadata = MakeUnique<firebase::storage::Metadata>();
	}
}
#endif

FFirebaseStorageMetadata::FFirebaseStorageMetadata()
{
	// Can't create the internal metadata here as it is going to be called
	// too early when UE4 creates the default object.
}

FFirebaseStorageMetadata::FFirebaseStorageMetadata(const FFirebaseStorageMetadata& Other)
{
	*this = Other;
}

FFirebaseStorageMetadata::FFirebaseStorageMetadata(FFirebaseStorageMetadata&& Other)
#if WITH_FIREBASE_STORAGE
	: Metadata(MoveTemp(Other.Metadata))
#endif
{
}

FFirebaseStorageMetadata& FFirebaseStorageMetadata::operator=(const FFirebaseStorageMetadata& Other)
{
#if WITH_FIREBASE_STORAGE
	if (Other.Metadata)
	{
		if (Metadata)
		{
			*Metadata = *Other.Metadata;
		}
		else
		{
			Metadata = MakeUnique<firebase::storage::Metadata>(*Other.Metadata);
		}
	}
	else
	{
		Metadata.Reset(nullptr);
	}
#endif

	return *this;
}

FFirebaseStorageMetadata& FFirebaseStorageMetadata::operator=(FFirebaseStorageMetadata&& Other)
{
#if WITH_FIREBASE_STORAGE
	Metadata = MoveTemp(Other.Metadata);
#endif
	return *this;
}
FString FFirebaseStorageMetadata::GetBucket() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->bucket()) : TEXT("");
#else
	return {};
#endif
}

void	FFirebaseStorageMetadata::SetCachControl(const FString& cache_control)
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		Metadata->set_cache_control(TCHAR_TO_UTF8(*cache_control));
	}
#endif
}

FString FFirebaseStorageMetadata::GetCacheControl() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->cache_control()) : FString();
#else
	return {};
#endif
}

void	FFirebaseStorageMetadata::SetContentDisposition(const FString& disposition)
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		Metadata->set_content_disposition(TCHAR_TO_UTF8(*disposition));
	}
#endif
}

FString FFirebaseStorageMetadata::GetContentDisposition() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->content_disposition()) : TEXT("");
#else
	return {};
#endif
}

void	FFirebaseStorageMetadata::SetContentEncoding(const FString& encoding)
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		Metadata->set_content_encoding(TCHAR_TO_UTF8(*encoding));
	}
#endif
}

FString FFirebaseStorageMetadata::GetContentEncoding() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->content_encoding()) : TEXT("");
#else
	return {};
#endif
}

void	FFirebaseStorageMetadata::SetContentLanguage(const FString& language)
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		Metadata->set_content_language(TCHAR_TO_UTF8(*language));
	}
#endif
}

FString FFirebaseStorageMetadata::GetContentLanguage() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->content_language()) : TEXT("");
#else
	return {};
#endif
}

void	FFirebaseStorageMetadata::SetContentType(const FString& type)
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		Metadata->set_content_type(TCHAR_TO_UTF8(*type));
	}
#endif
}

FString FFirebaseStorageMetadata::ContentType() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->content_type()) : TEXT("");
#else 
	return {};
#endif
}

int64	FFirebaseStorageMetadata::GetCreationTime() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->creation_time() : 0;
#else 
	return 0;
#endif
}

int64	FFirebaseStorageMetadata::GetGeneration() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->generation() : 0;
#else 
	return 0;
#endif
}

int64	FFirebaseStorageMetadata::GetMetadataGeneration() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->metadata_generation() : 0;
#else 
	return 0;
#endif
}

FString FFirebaseStorageMetadata::GetName() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->name()) : TEXT("");
#else 
	return {};
#endif
}

FString FFirebaseStorageMetadata::GetPath() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->path()) : TEXT("");
#else 
	return {};
#endif
}

int64	FFirebaseStorageMetadata::GetSizeBytes() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->size_bytes() : 0;
#else 
	return 0;
#endif
}

int64	FFirebaseStorageMetadata::GetUpdatedTime() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->updated_time() : 0;
#else 
	return 0;
#endif
}

bool	FFirebaseStorageMetadata::IsValid() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? Metadata->is_valid() : false;
#else 
	return false;
#endif
}

FString FFirebaseStorageMetadata::GetMd5Hash() const
{
#if WITH_FIREBASE_STORAGE
	return Metadata ? UTF8_TO_TCHAR(Metadata->md5_hash()) : TEXT("");
#else 
	return {};
#endif
}

TMap<FString, FString> FFirebaseStorageMetadata::GetCustomMetadata() const
{
#if WITH_FIREBASE_STORAGE
	TMap<FString, FString> CustomMeta;

	if (Metadata)
	{
		const auto* const RawMeta = Metadata->custom_metadata();

		for (const auto& MetaElem : *RawMeta)
		{
			CustomMeta.Emplace(UTF8_TO_TCHAR(MetaElem.first.c_str()), UTF8_TO_TCHAR(MetaElem.second.c_str()));
		}
	}

	return CustomMeta;
#else 
	return {};
#endif
}

UFirebaseStorageReference* FFirebaseStorageMetadata::GetReference() const
{
#if WITH_FIREBASE_STORAGE
	CreateInternal();
	if (Metadata)
	{
		UFirebaseStorageReference* const Ref = NewObject<UFirebaseStorageReference>();

		Ref->Reference = Metadata->GetReference();

		return Ref;
	}
#endif

	return nullptr;
}

#if WITH_FIREBASE_STORAGE
FFirebaseStorageMetadata::operator firebase::storage::Metadata& ()
{
	CreateInternal();
	return *Metadata;
}

FFirebaseStorageMetadata::operator const firebase::storage::Metadata& () const
{
	CreateInternal();
	return *Metadata;
}
#endif


#if WITH_FIREBASE_STORAGE
class FStorageListener final : public firebase::storage::Listener
{
public:
	FStorageListener(
		const FFirebaseStorageControllerCallback& OnProgress,
		const FFirebaseStorageControllerCallback& OnPaused
	) 
		: OnProgressEvent(OnProgress)
		, OnPausedEvent(OnProgress)
	{
	}
	virtual ~FStorageListener()
	{
	}
	virtual void OnPaused(firebase::storage::Controller* controller)
	{
		if (OnPausedEvent.IsBound())
		{
			FFirebaseStorageController Controller;
			*Controller.Controller = *controller;
			
			AsyncTask(ENamedThreads::GameThread, [Local = this->OnPausedEvent, Controller = MoveTemp(Controller)]() mutable -> void
			{
				Local.ExecuteIfBound(Controller);
			});
		}
	}
	virtual void OnProgress(firebase::storage::Controller* controller)
	{
		if (OnProgressEvent.IsBound())
		{
			FFirebaseStorageController Controller;
			*Controller.Controller = *controller;
			
			const FFirebaseStorageControllerCallback& Local = OnProgressEvent;
			AsyncTask(ENamedThreads::GameThread, [Local, Controller = MoveTemp(Controller)]() mutable -> void
			{
				Local.ExecuteIfBound(Controller);
			});
		}
	}
private:
	FFirebaseStorageControllerCallback OnProgressEvent;
	FFirebaseStorageControllerCallback OnPausedEvent;
};
#endif

FFirebaseStorageController::FFirebaseStorageController()
#if WITH_FIREBASE_STORAGE
	: Controller(MakeShared<firebase::storage::Controller, ESPMode::ThreadSafe>())
#endif
{
}

FFirebaseStorageController::FFirebaseStorageController(const FFirebaseStorageController& Other)
{
	*this = Other;
}

FFirebaseStorageController::FFirebaseStorageController(FFirebaseStorageController&& Other)
{
	*this = MoveTemp(Other);
}

FFirebaseStorageController& FFirebaseStorageController::operator=(const FFirebaseStorageController& Other)
{
#if WITH_FIREBASE_STORAGE
	Controller = Other.Controller;
#endif
	return *this;
}

FFirebaseStorageController& FFirebaseStorageController::operator=(FFirebaseStorageController&& Other)
{
#if WITH_FIREBASE_STORAGE
	Controller = MoveTemp(Other.Controller);
#endif
	return *this;
}

#if WITH_FIREBASE_STORAGE

bool FFirebaseStorageController::Pause()
{
	return Controller ? Controller->Pause() : false;
}

bool FFirebaseStorageController::Resume()
{
	return Controller ? Controller->Resume() : false;
}

bool FFirebaseStorageController::Cancel()
{
	return Controller ? Controller->Cancel() : false;
}

bool FFirebaseStorageController::IsPaused() const
{
	return Controller ? Controller->is_paused() : false;
}

int64 FFirebaseStorageController::BytesTransferred() const
{
	return Controller ? (int64)Controller->bytes_transferred() : 0;
}

int64 FFirebaseStorageController::TotalByteCount() const
{
	return Controller ? (int64)Controller->total_byte_count() : 0;
}

UFirebaseStorageReference* FFirebaseStorageController::GetReference() const
{
	if (!Controller)
	{
		return nullptr;
	}

	UFirebaseStorageReference* const Ref = NewObject<UFirebaseStorageReference>();

	Ref->Reference = Controller->GetReference();

	return Ref;
}

bool FFirebaseStorageController::IsValid() const
{
	return Controller && Controller->is_valid();
}

#endif // WITH_FIREBASE_STORAGE

UFirebaseStorageReference* UFirebaseStorageReference::Child(const FString& path) const
{
#if WITH_FIREBASE_STORAGE
	UFirebaseStorageReference* const ChildReference = NewObject<UFirebaseStorageReference>();

	ChildReference->Reference = Reference.Child(TCHAR_TO_UTF8(*path));

	return ChildReference;
#else
	return nullptr;
#endif
}

void UFirebaseStorageReference::Delete(FFirebaseStorageCallback Callback)
{
#if WITH_FIREBASE_STORAGE
	Reference.Delete().OnCompletion([Callback = MoveTemp(Callback)](const firebase::Future<void>& Future) mutable -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to delete Storage Reference.")
				TEXT(" Code: %d. Message: %s"), Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Error, Callback = MoveTemp(Callback)]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif
}

FString UFirebaseStorageReference::GetBucket()
{
#if WITH_FIREBASE_STORAGE
	return UTF8_TO_TCHAR(Reference.bucket().c_str());
#else
	return {};
#endif
}

FString UFirebaseStorageReference::GetFullPath()
{
#if WITH_FIREBASE_STORAGE
	return UTF8_TO_TCHAR(Reference.full_path().c_str());
#else
	return {};
#endif
}

void UFirebaseStorageReference::GetFile
(
	const FString& Path, 
	FFirebaseStorageController& Controller, 
	const FFirebaseStorageInt64Callback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener = 
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.GetFile(TCHAR_TO_UTF8(*Path), Listener.Get(), Controller.Controller.Get()).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Callback, Listener](const firebase::Future<size_t>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to get file. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			const size_t Size = Future.result() ? *Future.result() : 0;
			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Size]() -> void
			{
				Callback.ExecuteIfBound(Error, (int64)Size);
			});
		}
	});
#endif
}

void UFirebaseStorageReference::GetBytes
(
	const int64 BufferSize,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageBinaryCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	void* const Buffer = FMemory::Malloc(BufferSize);

	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.GetBytes
	(
		Buffer,
		BufferSize,
		Listener.Get(),
		Controller.Controller.Get()
	).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Callback, Listener = MoveTemp(Listener), Buffer](const firebase::Future<size_t>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to get bytes. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			const size_t Size = Future.result() ? *Future.result() : 0;
			AsyncTask(ENamedThreads::GameThread, [Callback, Buffer, Error, Size]() -> void
			{
				TArray<uint8> Data((uint8*)Buffer, Size);
				
				Callback.ExecuteIfBound(Error, Data);
				
				FMemory::Free(Buffer);
			});
		}
		else
		{
			FMemory::Free(Buffer);
		}
	});
#endif
}

void UFirebaseStorageReference::GetDownloadUrl(const FFirebaseStorageStringCallback& Callback)
{
#if WITH_FIREBASE_STORAGE
	if (!Callback.IsBound())
	{
		return;
	}

	Reference.GetDownloadUrl().OnCompletion([Callback](const firebase::Future<std::string>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to download URL. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FString Url = Future.result() ? UTF8_TO_TCHAR(Future.result()->c_str()) : TEXT("");

			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Url = MoveTemp(Url)]() -> void
			{
				Callback.ExecuteIfBound(Error, Url);
			});
		}		
	});
#endif
}

void UFirebaseStorageReference::GetMetadata(const FFirebaseStorageMetadataCallback& Callback)
{
#if WITH_FIREBASE_STORAGE
	if (!Callback.IsBound())
	{
		return;
	}

	Reference.GetMetadata().OnCompletion([Callback](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to get Metadata. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Metadata = MoveTemp(Metadata)]() -> void
			{
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}


void UFirebaseStorageReference::UpdateMetadata(const FFirebaseStorageMetadata& InMetadata,
	const FFirebaseStorageMetadataCallback& Callback)
{
#if WITH_FIREBASE_STORAGE
	Reference.UpdateMetadata(InMetadata.IsValid() ? InMetadata : firebase::storage::Metadata())
		.OnCompletion([Callback](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to update Metadata. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Metadata = MoveTemp(Metadata)]() -> void
			{
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}

FString UFirebaseStorageReference::GetName()
{
#if WITH_FIREBASE_STORAGE
	return UTF8_TO_TCHAR(Reference.name().c_str());
#else
	return { };
#endif
}

UFirebaseStorageReference* UFirebaseStorageReference::GetParent()
{
#if WITH_FIREBASE_STORAGE
	UFirebaseStorageReference* const Ref = NewObject<UFirebaseStorageReference>();

	Ref->Reference = Reference.GetParent();

	return Ref;
#else
	return nullptr;
#endif
}

void UFirebaseStorageReference::PutBytes
(
	const TArray<uint8>& Bytes,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageMetadataCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.PutBytes
	(
		Bytes.GetData(),
		Bytes.Num(),
		Listener.Get(),
		Controller.Controller.Get()
	).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Callback, Listener](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to put bytes. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Metadata]() -> void
			{				
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}

void UFirebaseStorageReference::PutBytes
(
	const TArray64<uint8>& Bytes,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageMetadataCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.PutBytes
	(
		Bytes.GetData(),
		Bytes.Num(),
		Listener.Get(),
		Controller.Controller.Get()
	).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Controller = Controller.Controller, Callback, Listener](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to put bytes. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Listener, Callback, Error, Metadata = MoveTemp(Metadata)]() -> void
			{				
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}

void UFirebaseStorageReference::PutBytes
(
	const TArray<uint8>& Bytes,
	const FFirebaseStorageMetadata& InMetadata,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageMetadataCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.PutBytes
	(
		Bytes.GetData(),
		Bytes.Num(),
		InMetadata.IsValid() ? InMetadata : firebase::storage::Metadata(),
		Listener.Get(),
		Controller.Controller.Get()
	).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Controller = Controller.Controller, Callback, Listener](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to put bytes. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Metadata = MoveTemp(Metadata)]() -> void
			{				
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}

void UFirebaseStorageReference::PutFile
(
	const FString& Path,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageMetadataCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE

	if (!Reference.is_valid())
	{
		UE_LOG(LogFirebaseStorage, Error, TEXT("Called PutFile() with an invalid reference."));
		Callback.ExecuteIfBound(EFirebaseStorageError::Cancelled, {});
		return;
	}

	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	Reference.PutFile
	(
		TCHAR_TO_UTF8(*Path),
		Listener.Get(),
		Controller.Controller.Get()
	).OnCompletion(
		// We capture the Listener here so it outlives the result callback
		[Callback, Controller = Controller.Controller, Listener](const firebase::Future<firebase::storage::Metadata>& Future) mutable -> void
	{
		FFirebaseStorageMetadata _Metadata;
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();

		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to put file. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		else if (Future.result() && _Metadata.Metadata)
		{
			*_Metadata.Metadata = *Future.result();
		}

		AsyncTask(ENamedThreads::GameThread, [Controller, Listener, Callback, Error, _Metadata = MoveTemp(_Metadata)]() mutable -> void
		{				
			Callback.ExecuteIfBound(Error, _Metadata);
			Listener = nullptr;
			Controller = nullptr;
		});

		Controller = nullptr;
		Listener = nullptr;
	});
#endif
}

void UFirebaseStorageReference::PutFile
(
	const FString& Path,
	const FFirebaseStorageMetadata& InMetadata,
	FFirebaseStorageController& Controller,
	const FFirebaseStorageMetadataCallback& Callback,
	const FFirebaseStorageControllerCallback& OnProgress,
	const FFirebaseStorageControllerCallback& OnPaused
)
{
#if WITH_FIREBASE_STORAGE
	if  (!Reference.is_valid())
	{
		UE_LOG(LogFirebaseStorage, Error, TEXT("Called PutFile() with an invalid reference."));
		Callback.ExecuteIfBound(EFirebaseStorageError::Cancelled, {});
		return;
	}

	TSharedPtr<FStorageListener, ESPMode::ThreadSafe> Listener =
		MakeShared<FStorageListener, ESPMode::ThreadSafe>(OnProgress, OnPaused);

	(InMetadata.IsValid() ? Reference.PutFile
	(
		TCHAR_TO_UTF8(*Path),
		InMetadata,
		Listener.Get(),
		Controller.Controller.Get()
	) : 
	Reference.PutFile
	(
		TCHAR_TO_UTF8(*Path),
		Listener.Get(),
		Controller.Controller.Get()
	)).OnCompletion(
		// We capture the Listener and controller here so they outlive the result callback
		[Controller, Callback, Listener](const firebase::Future<firebase::storage::Metadata>& Future) -> void
	{
		const EFirebaseStorageError Error = (EFirebaseStorageError)Future.error();
		if (Error != EFirebaseStorageError::None)
		{
			UE_LOG(LogFirebaseStorage, Error, TEXT("Failed to put file. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}
		else
		{
			UE_LOG(LogFirebaseStorage, Log, TEXT("File Uploaded."));
		}

		if (Callback.IsBound())
		{
			FFirebaseStorageMetadata Metadata;

			if (Future.result() && Metadata.Metadata)
			{
				*Metadata.Metadata = *Future.result();
			}

			AsyncTask(ENamedThreads::GameThread, [Controller, Listener, Callback, Error, Metadata = MoveTemp(Metadata)]() -> void
			{				
				Callback.ExecuteIfBound(Error, Metadata);
			});
		}
	});
#endif
}

bool UFirebaseStorageReference::IsValid() const
{
#if WITH_FIREBASE_STORAGE
	return Reference.is_valid();
#else
	return false;
#endif
}


