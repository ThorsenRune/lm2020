//testing.js		only for testing purpose

function testGetData(){
	var oElem=prot.ElementByName( display.idVarName1.value)
	mTX_GetReq(oElem)
}
function testSetData(){
	var oElem=prot.ElementByName( display.idVarName1.value)
	oElem.Vector=utils.String2Array(idInput.value);
	mTX_SetReq(oElem);
}

function writeButton() {
	var a=idInput.value.split(',');
	for (var i=0;i<a.length;i++){
		var  senddata =parseInt(a[i])
		serial.send(senddata);
	}
}
function mFromDevice(aRXData){
	var elRes=document.getElementById("idData");
	elRes.innerHTML  = aRXData ;
}
var mMessage=function(txt,clear){
	var el=idDebugText;
	if (clear) el.value='';
	el.value=el.value+txt+"\n";
}
var mDebugSetInput=function(oElem){	//Show data in input
	idInput.value=(''+oElem.Vector).replaceAll(',',' , ')
}


var mIsVisible=function(el){
	var vis=el.clientHeight>100
	return vis
}
