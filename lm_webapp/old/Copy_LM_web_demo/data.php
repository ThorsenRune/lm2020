<?PHP		//Start on mMain
ini_set('display_errors', 1);			
ini_set('display_startup_errors', 1);
header("Access-Control-Allow-Origin: *");		//190626 Enabling CORS
error_reporting(E_ALL| E_STRICT); 		//Stop on all errors for debugging
class prog{	
	public static function mMain()
	{		
		//echo('testing data.php<hr>');
		//var_dump(prog::testdata());
		//prog::data_save(prog::testdata());
		//$s=prog::data_load()	;
		//$oVars=prog::data_load();	// o->Protocol, o->aVal[name]=[data array]
		$s=prog::mGetData();
		if (isset($s->data->sFileName)) prog::$DATABASE=$s->data->sFileName;
		if($s->cmd=='save'){
			$s->cmd='saved';
			prog::data_save($s->data);
		} else if($s->cmd=='load'){
			$s->cmd='loaded';
			$s->data=prog::data_load();
		} else if($s->cmd=='swap'){		//Get previous file data and write new
			$old=prog::data_load();
			prog::data_save($s->data);
			$s->cmd='old';
			$s->data=$old;
			
		}
		die(json_encode($s));		//Return data to javascript
	}
	public static function mGetData(){
		if (!isset($_POST['data'])) return null;
		$d=$_POST['data'];			//Get data
		$d=json_decode($d);
		return $d;
	}
	public static function mFileName(){
		return prog::$datdir.prog::$DATABASE;
		
	}
	public static $datdir ='./data/';
	public static $DATABASE=  'data.txt';
	public static function data_load()	{		//Loads current protocol object with data  
		return file_exists(prog::mFileName()) ? json_decode(file_get_contents(prog::mFileName())) : false;
	}
	public static function data_save($db)
	{
		//file_put_contents(prog::mFileName(), json_encode($db, JSON_UNESCAPED_UNICODE ));
		file_put_contents(prog::mFileName(), json_encode($db, JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT));
		//But we dont want to force objects which will corrupt  arrays
		//file_put_contents(prog::mFileName(), json_encode($db, JSON_FORCE_OBJECT | JSON_UNESCAPED_UNICODE | JSON_PRETTY_PRINT));
	}
	
}
prog::mMain();

?>