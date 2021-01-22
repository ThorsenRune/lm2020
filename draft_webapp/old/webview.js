//	to remote debug %LOCALAPPDATA%\Android\sdk\platform-tools\adb start-server
//	To fill a dropdown use mFillDataList ..Object.keys(oWatch.prot)
'use strict';
if(!And) 	var And={}
var bRelay= (typeof Android =='undefined') ;//Android is an object injected from the  Webview when using the slider.
//Communication constants


const popup={
	 mMessage:function(str){
		this.mShow(true,window.idText,mAccept)
		window.idSettings.hidden=true;		//Hide the settings window
		var e=document.getElementById('idText');
		e.readOnly=true									//Disable input, ie. no keyboard
		if (!e) alert('Missing message window');			//No message window present
		if (!str){e.value='';this.mShow(false)}			//191114 Close the window if message is null
		else e.value=str+'\n'+e.value ;// Show the command
		function mAccept(){
			e.value=''
		}
	} 
	,mSettings:function(){
		this.mShow(true,window.idSettings,mAccept)
		function mAccept(){
			mDisp2Var(prot)
 		}
		mVar2Disp(prot);
	}
	,mShow:function(bVis,elActive,callback){
		var that=this;
		if (bVis==false) {
			window.idPopUp.hidden=true
		}else if (window.idPopUp.hidden){
			window.idSettingsCancel.onclick=function(){that.mShow(false)}
			window.idText.onblur=window.idSettingsCancel.onclick;
			window.idText.hidden=true; 
			window.idSettings.hidden=true; 
			window.idPopUp.hidden=false
			elActive.hidden=false
			elActive.focus()
			window.idTextClose.ontouchstart=
			window.idTextClose.onclick=function(){
				callback();
				that.mShow(false);
			}
		}

	}
}

 

 
function mWidgetSet(oWX){
	if (!oWX||!oWX.id) return
	if (!window[oWX.id]) return
	var elem=window[oWX.id].querySelector('input')
	if (elem){	//Slider element
		if (oWX.Range()){
			slider.mSettings(elem
			,oWX.Range()
			,oWX.Unit()
			,oWX.Alias);	//Set the slider range in units
			slider.mValue(elem,oWX.Value())
		}
	}
}

 

function mFocus(elem){
	mVar2DropSel(elem)
}
function mVar2DropSel(ctrl){	//Show the currently selected control
	window.idVarName1.control=ctrl
	window.idVarName1.value=mVarName(ctrl)
	prot.ActiveVar=mVarName(ctrl);
	prot.ActiveWx=ctrl;
	if (!prot.ActiveWx.id) prot.ActiveWx=prot.ActiveWx.parentElement
}
function mDDSetVarName(){
	var c=window.idVarName1.control;
	mVarName(c,window.idVarName1.value)		//The control element holds the variable name
	display.redraw()
	//mWidgets2ProtObj()
}
function mCurrentCtrlId(){	//Return ID of current control
	if(!window.idVarName1.control) return null;
	var cid=window.idVarName1.control.parentElement.id;
	return cid;
}

function mVarName(ctrl,newname){
	var id=ctrl.parentElement.id
	var oWX=prot.oWX(id);
	if (!oWX) return
	if (newname!==undefined){		//A newname has been given 
		prot.oWX(id).VarName(newname);
		prot.Poke()
	} 
	return prot.oWX(id).VarName();
}

 
 function mVar2Disp(prot) {	//window.opener.prot
		var oWX=prot.oWX();
		if (oWX==null) {window.idPopUp.hidden='true';return}
		window.idVarName.innerText	=oWX.VarName();
		if(oWX.Index())
		window.idIdx.value			=oWX.Index()
		if(oWX.Value())
		window.idValue.value		=oWX.Value()

		window.idDescr.value		=oWX.Alias
		 
		window.idUnit.value			=oWX.Unit()
		window.idFactor.value		=oWX.Factor()
		window.idOffset.value		=oWX.Offset()
		var r=oWX.Range() ;
		if (r) {
			window.idDispMax.value		=r[1];
			window.idDispMin.value		=r[0];
		}
	}
	function mDisp2Var(prot) {	//window.opener.prot
		var sName=prot.ActiveVar;
		var oWX=prot.oWX();			//Widget object
		oWX.Alias=(window.idDescr.value)
 
		oWX.Index(Number(window.idIdx.value) )
		oWX.Value(Number( window.idValue.value))
		oWX.Unit(window.idUnit.value)
		oWX.Factor(Number(window.idFactor.value))
		oWX.Offset(Number(window.idOffset.value))
		var r=[Number(window.idDispMin.value)
				,Number(window.idDispMax.value)]
		oWX.Range(r);
		mWidgetSet(oWX);//Refresh the widget
		prot.Poke();
	}
	