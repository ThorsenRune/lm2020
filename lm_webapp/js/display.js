/*
	Display updating functions 
	mostly as callbacks
*/
'use strict';	//Require variable declarations

var display={};		//Entry Methods doing GUI update 
display.init=function(){
	display.idVarName1	=window.idVarName1;		//Variable dropdown list
	display.idStatus	=window.idStatus;
	display.idDispMax	=window.idDispMax
	display.idDispMin	=window.idDispMin;
	display.idFactor	=window.idFactor;
	display.idUnit		=window.idUnit;
	display.idDescr		=window.idDescr
	display.idValue		=window.idValue
	display.idIdx		=window.idIdx
	display.ActiveWidget=document.activeElement;			//Active control
}

 
display.redraw=function(){	//Redraw the display
	var e=display.idStatus;
	if (serial.isReady()){
		e.style.background=''
	} else {
		e.style.background='red'
	}	
	for (var i = 0; i < slider.inputs.length; i++) {		//Set widgets
		var sId=slider.id(i)
		var oWX=prot.oWX(sId)
		if (!oWX ) return;
 
		if(!oWX.Range) debugger
		mWidgetSet(oWX)
	}	
	oWX=prot.oWX(signal.id())		//Setup the signal
	//Make a dropdown selector for the protocol elements
	lib.mFillDataList(display.idVarName1,Object.keys(prot.oProtElemVar));	// To gui
	display.doRedraw=false;
}	 
display.refresh=	function(){			//Will refresh controls in the display

	if (!prot.oWidgets)return
 	var oWXs=prot.oWidgets;
	var aIDs=Object.keys(oWXs)
	for (var i = 0; i < aIDs.length; i++){
		var sId=aIDs[i]
		var c=window[sId]
		var oWX=prot.mWX(sId)
		if (!mIsVisible(c)) {				//Skip non visible elements
			
		}else if (c.id==signal.id()){		//Signal panel
			if (oWX.VectorUnits()){			//Only if valid
				signal.Title(oWX.Alias)
				var X=oWX.VectorUnits()
				var r=oWX.Range()
				signal.mPlotVector(X,r[0],r[1],oWX.Unit())
				oWX.oProtElemVar.peekdata=true;			//Read value from device
			}
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



function mFocus(elem){
	mVar2DropSel(elem)
}
function mVar2DropSel(ctrl){	//Show the currently selected control
	display.ActiveWidget=ctrl
	display.idVarName1.value=mVarName(ctrl)
	prot.ActiveVar=mVarName(ctrl);
	prot.ActiveWx=ctrl;
	if (!prot.ActiveWx.id) prot.ActiveWx=prot.ActiveWx.parentElement
}
function mDDSetVarName(){
	var c=display.ActiveWidget;
	if (display.ActiveWidget)
		mVarName(c,display.idVarName1.value)		//The control element holds the variable name
	display.redraw()
	//mWidgets2ProtObj()
}
function mCurrentCtrlId(){	//Return ID of current control
	if(!display.ActiveWidget) return null;
	var cid=display.ActiveWidget.parentElement.id;
	return cid;
}

function mVarName(ctrl,newname){
	var id=ctrl.parentElement.id
	var oWX=prot.oWX(id);
	if (!oWX) return
	if (newname!==undefined){		//A newname has been given 
		prot.oWX(id).VarName(newname);
	} 
	return prot.oWX(id).VarName();
}

 
 function mVar2Disp(prot) {	//window.opener.prot
		var oWX=prot.oWX();
		if (oWX==null) {display.idPopUp.hidden='true';return}
		display.idVarName.innerText	=oWX.VarName();
		if(oWX.Index())
		display.idIdx.value			=oWX.Index()
		if(oWX.Value())
		display.idValue.value		=oWX.Value()

		display.idDescr.value		=oWX.Alias
		 
		display.idUnit.value			=oWX.Unit()
		display.idFactor.value		=oWX.Factor()
		display.idOffset.value		=oWX.Offset()
		var r=oWX.Range() ;
		if (r) {
			display.idDispMax.value		=r[1];
			display.idDispMin.value		=r[0];
		}
	}
	function mDisp2Var(prot) {	//window.opener.prot
		var sName=prot.ActiveVar;
		var oWX=prot.oWX();			//Widget object
		oWX.Alias=(display.idDescr.value)
 
		oWX.Index(Number(display.idIdx.value) )
		oWX.Value(Number( display.idValue.value))
		oWX.Unit(display.idUnit.value)
		oWX.Factor(Number(display.idFactor.value))
		oWX.Offset(Number(display.idOffset.value))
		var r=[Number(display.idDispMin.value)
				,Number(display.idDispMax.value)]
		oWX.Range(r);
		mWidgetSet(oWX);//Refresh the widget
		prot.pokedata();
	}
	


const popup={
	 mMessage:function(str){
		this.mShow(true,display.idText,mAccept)
		display.idSettings.hidden=true;		//Hide the settings window
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
		this.mShow(true,display.idSettings,mAccept)
		function mAccept(){
			mDisp2Var(prot)
 		}
		mVar2Disp(prot);
	}
	,mShow:function(bVis,elActive,callback){
		var that=this;
		if (bVis==false) {
			display.idPopUp.hidden=true
		}else if (display.idPopUp.hidden){
			display.idSettingsCancel.onclick=function(){that.mShow(false)}
			display.idText.onblur=display.idSettingsCancel.onclick;
			display.idText.hidden=true; 
			display.idSettings.hidden=true; 
			display.idPopUp.hidden=false
			elActive.hidden=false
			elActive.focus()
			display.idTextClose.ontouchstart=
			display.idTextClose.onclick=function(){
				callback();
				that.mShow(false);
			}
		}
	}
}
var nSubCounter=10;


	 