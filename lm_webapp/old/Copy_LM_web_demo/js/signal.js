//Plotting the signal
"use strict";
var eSVG=document.querySelector("svg")
var path1=eSVG.querySelector('#idSignal');
var lMidline=eSVG.querySelector('#idMidLine');


var signal={
	id:function(n){
		return eSVG.parentElement.id;
	}
	,Title:function(str){
		window.idSignalTitle.innerHTML=str;
	}

}

function mSignalInit(){
	var len=100;
	var ampl=1000;
	eSVG.ontouchstart	= function(){mFocus(this)}
	eSVG.onclick		= function(){mFocus(this)}
}
function mPlotVector1(Y,min,max,units){
	var len=Y.length
	if(len<2) return;
	eSVG.setAttribute('preserveAspectRatio', 'none');
	//viewBox = <min-x> <min-y> <width> <height>
	var range=max-min;
	eSVG.setAttribute('viewBox',"0  0 "+len+" "+range);
	var top=max			//Flip yaxis
	//Center origo at midline
	//Centerline 
	lMidline.setAttribute("d"," M0,"+(top-0)+" L"+len+","+(top-0));
	var path= "M0,"+(top-Y[0]);
	for (var i = 1; i < len; i++) {
		path=path+" L"+i+","+(top-Y[i]);	//Flip yaxis
	}
	path1.setAttribute("d", path)
	window.idMaxVal.textContent=max+units;
	window.idMinVal.textContent=min+units;
}


function mPlotVector(Y,range){
	var len=Y.length
	if(len<2) return;
	if(!range)  range=2;
	eSVG.setAttribute('preserveAspectRatio', 'none');
	//viewBox = <min-x> <min-y> <width> <height>
	eSVG.setAttribute('viewBox',"0 0 "+len+" "+range);
	//Center origo at midline
	var a=0.5*range;
	//Centerline
	lMidline.setAttribute("d"," M0,"+a+" L"+len+","+(a));
	var path= "M0,"+(a-Y[0]);
	for (var i = 1; i < len; i++) {
		
		path=path+" L"+i+","+(a-Y[i]);	//Flip yaxis
	}
	path1.setAttribute("d", path)
	window.idMaxVal.textContent=a;
	window.idMinVal.textContent=-a;
}
