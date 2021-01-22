// TODO.js			//Descriptive file of what to do next
/*
 	  []  fix popup.mSettings() - check idPopUp - Shoud allow for editing the currently selected element/widget
		[]  fix the varsettings.html so it can be used for editing the protocol elements and widgets
	  []	Put documentation in \DOC
    []  Port protocol FW files in arduino see https://drive.google.com/drive/folders/185abc_MnY5sStTrSBwsle0lv1kH3KRYu
		todo: TodoProtocols let prot be an array, so we can connect more devices. E.g. prot[0] could be MeCFES1 and prot[1] eglove etc.
*/

TodoProtocols{
	let prot be an array, so we can connect more devices. E.g. prot[0] could be MeCFES1 and prot[1] eglove etc.
	oaProtocols = new cProtocol3[nDevices];


}

Functionalities to port from old Conversion
		[]	set a value by texinput box former. oUInput.mInputValue1
		[]	resizing panels  former. nPanelSizes
  	[]	blanking slider (horizontal) former
		[]	select other signal panels swiping forward backward to see filtered emg etc
		[]	Show Data - grid of datavalues that can be editied (optional)
		[]	"Set Device Mode" former   mBitFields_Show(); for starting stopping devices,modes, filter selection etc
		[]	mPrivileges select usermodes eg therapist, expert, administrator of setup
		[]	mSettingsDialog selecting active devices setupfiles, save datafiles bluetooth connection etc
		[]	mShowAbout - information
		[]	zoom: mZoom
		[]	scaling mScaleMaxStr
		[]	mAutoRange
note: bitfields - a 32 bit word is used for flags.
nMode.all_flags see GlobalData.h in firmware:https://docs.google.com/document/d/1afB7kv-D1lDcdTOnBufZNtuWIsrhLKDh6-ncjtmigwg/edit#bookmark=id.ezryzccd8h0u

=========== REFACTORING ===============
oWatch<===Widget

Refactoring ideas:{
	Modularity or Keeping functionalities separate.
	2. DRY -> Don’t Repeat Yourself
  Extract Function {
    Create a new function and name it for what it does.
    function x(){
     dosomething()
     -....
     function dosomething(){}
    }
  }
  Prefer Parameter Object
  {
      function fun(x,y)
      function fun(yxobj)
  }

	Codestyle
	    I prefer sCamelCase style with cMyClass, sString, nNumeber, oObject, eEnum


}
