# NBC_HW10 - 언리얼 모듈과 플러그인
#### 차지훈

## 플러그인 생성 방법
1. Unreal Editor의 상단 툴바에서 "Edit" - "Plugins" 선택
<img width="478" height="546" alt="image" src="https://github.com/user-attachments/assets/454170a7-9e08-46ff-8b01-6808ada98c36" />


2. Unreal Editor의 Plugins 창에서 Add 버튼을 선택
<img width="515" height="184" alt="image" src="https://github.com/user-attachments/assets/49c135dd-d9cc-4d5c-9fcd-b57fd9a9b27f" />


3. NewPlugin 생성 창에서 원하는 플러그인 형식을 선택하고 플러그인 이름 입력 후 "Create Plugin" 버튼 선택
<img width="1521" height="1143" alt="image" src="https://github.com/user-attachments/assets/ed1b7c41-461b-4dae-8ca0-746c161496fc" />
- Plugins의 폴더 내부에 입력된 이름의 Plugin과 Module이 생성된다.


4. C++ 클래스 추가 시 생성된 Module을 선택하여 해당 모듈에 클래스 추가
- "MyPlugin"이름으로 Plugin 생성 시 "MyPlugin" 모듈 자동 생성됨
<img width="1402" height="562" alt="image" src="https://github.com/user-attachments/assets/512de941-2cca-4cf1-9534-9cc9b534e080" />


---
## 플러그인 안에 추가 모듈 생성 방법
1. 생성된 Plugin 폴더 하위의 Source 폴더 열기
    - Ex) ...\MyUnrealProject\Plugins\MyPlugin\Source

  
2. Source 폴더 내부에 추가하고자 하는 모듈의 이름과 동일한 폴더 생성
    - Ex) ...\Plugins\MyPlugin\Source\MySub 
 <img width="98" height="23" alt="image" src="https://github.com/user-attachments/assets/b1c13255-c4b6-474f-add8-9bf7cf9c443a" />


3. 생성된 폴더 내부에 "Public", "Private" 폴더 생성 및 .Build.cs 파일 생성
    - .Build.cs 파일 이름은 모듈 이름과 동일하게 설정 (MySub.Build.cs)
 <img width="226" height="112" alt="image" src="https://github.com/user-attachments/assets/2720e0c7-2e1e-4233-bc4c-748f0b0f20d0" />


4. Build.cs 파일 작성
- 클래스 이름과 생성자 이름을 모듈 이름과 동일하게 작성

```
using UnrealBuildTool;
public class MySub : ModuleRules
{
	public MySub(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
            }
			);
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
            }
			);
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
				"Engine",
				"UMG",
			}
			);
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
      }
			);
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
```

5. Public 폴더 내부에 모듈 이름과 같은 .h 파일 생성 및 내용 작성
- 파일 이름은 모듈 이름과 같도록 생성 (MySub.h)
- 클래스 이름은 F + (ModuleName) + Module 으로 작성
```
#pragma once

#include "Modules/ModuleManager.h"

class FMySubModule : public IModuleInterface    // 모듈 이름 주의
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

```

6. Private 폴더 내부에 모듈 이름과 같은 .cpp 파일 생성 및 내용 작성
- 파일 이름은 모듈 이름과 같도록 생성 (MySub.cpp)
- 내용 작성 시 모듈 이름과 클래스 이름을 주의하여 작성
```
#include "MySub.h"    // 헤더 파일 이름 주의

#define LOCTEXT_NAMESPACE "FMySubModule"

void FMySubModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FMySubModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMySubModule, MySub)
```


7. .uproject의 Visual Studio Project File 재생성
<img width="224" height="81" alt="image" src="https://github.com/user-attachments/assets/0072ddd5-6874-4cd7-80e1-3da0dfa961b4" />


8. .uplugin 파일 내용 수정
- Modules에 모듈 이름 추가
```
	"Modules": [
		{
			"Name": "MyPlugin",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		}. 
		{
			"Name": "MySub",
			"Type": "Runtime",
			"LoadingPhase": "Default"
		}
	]
```
   
