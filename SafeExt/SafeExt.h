/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri May 04 00:02:42 2012
 */
/* Compiler settings for E:\S\SafeNetwork\Src\SafeExt\Src\SafeExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SafeExt_h__
#define __SafeExt_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IQuickOpen_FWD_DEFINED__
#define __IQuickOpen_FWD_DEFINED__
typedef interface IQuickOpen IQuickOpen;
#endif 	/* __IQuickOpen_FWD_DEFINED__ */


#ifndef __QuickOpen_FWD_DEFINED__
#define __QuickOpen_FWD_DEFINED__

#ifdef __cplusplus
typedef class QuickOpen QuickOpen;
#else
typedef struct QuickOpen QuickOpen;
#endif /* __cplusplus */

#endif 	/* __QuickOpen_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IQuickOpen_INTERFACE_DEFINED__
#define __IQuickOpen_INTERFACE_DEFINED__

/* interface IQuickOpen */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IQuickOpen;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5F23781A-2D56-4A86-B54F-F35F212B0C81")
    IQuickOpen : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IQuickOpenVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQuickOpen __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQuickOpen __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQuickOpen __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IQuickOpen __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IQuickOpen __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IQuickOpen __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IQuickOpen __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IQuickOpenVtbl;

    interface IQuickOpen
    {
        CONST_VTBL struct IQuickOpenVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQuickOpen_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQuickOpen_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQuickOpen_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQuickOpen_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IQuickOpen_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IQuickOpen_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IQuickOpen_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IQuickOpen_INTERFACE_DEFINED__ */



#ifndef __SAFEEXTLib_LIBRARY_DEFINED__
#define __SAFEEXTLib_LIBRARY_DEFINED__

/* library SAFEEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SAFEEXTLib;

EXTERN_C const CLSID CLSID_QuickOpen;

#ifdef __cplusplus

class DECLSPEC_UUID("B0597F7E-06FF-4A31-9C2C-11483CE7F30E")
QuickOpen;
#endif
#endif /* __SAFEEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
