#include "AEConfig.h"

#ifndef AE_OS_WIN
	#include "AE_General.r"
#endif

resource 'PiPL' (16000) {
	{	/* array properties: 7 elements */
		/* [1] */
		Kind {
			AEGP
		},
		/* [2] */
		Name {
            CODEC_NAME
		},
		/* [3] */
		Category {
			"General Plugin"
		},
		/* [4] */
		Version {
			65536
		},
		/* [5] */
#ifdef AE_OS_WIN
	#ifdef AE_PROC_INTELx64
		CodeWin64X86 {"EntryPointFunc"},
	#else
		CodeWin32X86 {"EntryPointFunc"},
	#endif
#else
	#ifdef AE_OS_MAC
		#ifdef AE_PROC_ARM64
			CodeMacARM64 {"EntryPointFunc"},
		#elif defined(AE_PROC_INTELx64)
			CodeMacIntel64 {"EntryPointFunc"},
		#elif defined(AE_PROC_INTEL)
			CodeMacIntel32 {"EntryPointFunc"},
		#endif
	#endif
#endif
	}
};

