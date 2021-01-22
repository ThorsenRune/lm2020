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
		prot.mGetStatus();
	}
}
var nSubCounter=1;
var mCyclicRefresh=function(interval){
	//fnname = arguments.callee.name;
 	if(oWatch)
	if (oWatch.nPeriod>0){
		if (bAndroid) mAndExchange();	//Data exhange to Android App
		
		mWidgetsRefresh()
		nSubCounter=nSubCounter-1;
		if (nSubCounter<0){
			nSubCounter=5;
			mStatusCheck();
			prot.mDataExchange();
		}
		setTimeout(function() {
			requestAnimationFrame(mCyclicRefresh);
		},  oWatch.nPeriod);
	}
}
 
	 
var mStatusCheck=function(){
	var state=prot.info().ProtState;
	var e=window.idStatus;
	if (state!=prot.lastState){	//Redraw display
		prot.mSetup();		//Prepare protocol changes

		mDispRedraw()
		if (state=="kProtReady"){
			e.style.background=''
		} else {
			e.style.background='red'
		}	
		 
	}
	prot.lastState=prot.info().ProtState;
}
var mWidgetsRefresh=function(){

 	var oWXs=prot.oWidgets;
	var aIDs=Object.keys(oWXs)
	for (var i = 0; i < aIDs.length; i++){
//todo:enable signal
		var sId=aIDs[i]
		var c=window[sId]
		var oWX=prot.mWX(sId)
		if (c.id==signal.id()){
		//	var X=oWX.oVarData.Vector;
 		//	mPlotVector(X.sub(X.mean()),X.max()-X.min())
			signal.Title(oWX.Alias)
			var X=oWX.VectorUnits()
			//mPlotVector(X.sub(X.mean()),X.max()-X.min())
			var r=oWX.Range()
			mPlotVector1(X,r[0],r[1],oWX.Unit())
			oWX.oVarData.Peek=true;
		} else {  //Slider
			var el=window[oWX.id].querySelector('input')
			if (!oWX){
			
			}else if (el.bPokeValue)		{	//Request to write data to device
				var v=slider.mValue(el);	//Get the sliders value
				oWX.Value(v);
				el.bPokeValue=false;
			}else if (oWX.Value){
				var v=oWX.Value() ;
				slider.mValue(el,v)	//Set the sliders value
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

function mDispRedraw(){	//Redraw the display
	for (var i = 0; i < slider.inputs.length; i++) {		//Set widgets
		var sId=slider.id(i)
		var oWX=prot.oWX(sId)
		if (!oWX ) return;
		if(!oWX.Range) debugger
		mWidgetSet(oWX)
	}	
	oWX=prot.oWX(signal.id())		//Setup the signal
	mFillDataList(window.idVarName1,
		Object.keys(prot.oVarData));	// To gui
}
function mInit(){
	//Change the default filename for dataexchange
	var datafile=location.hash.replace('#','')
	if (datafile.length>2) prot.sFileName=datafile+'.txt';
	window.idVarName1.onchange=mDDSetVarName;
	prot.mInitialize(function(){	//contains async call
		mSignalInit();
		mCyclicRefresh();
	});
	
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
	mDispRedraw()
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
	