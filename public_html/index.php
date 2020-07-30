<!DOCTYPE html>
<html>
    <head>
        <title> Homepage Neil Schroeder (UMN) </title>
        <meta charset="utf-8">
 <!-- begin php script -->
<?php 
//functions:
function getBaseDirPrefix(){
    $ret = '';
    $this_dir = __DIR__;
    if( basename($current_dir) != "public_html" ) {
        // couldn't get the while logic to work, so here's a big ol' for loop 
        for($i = 0; $i < 10; $i++){ 
            if(basename($this_dir) != "public_html"){
                $ret = $ret.'../';
            }   
            else{
                break 1;
            }
            $this_dir = dirname($this_dir);
        }
    }
    return $ret;
}

function printUlLiFromArray($this_dir, $except_dirs){
    //prints the <li> </li> statements for directories in $this_dir_list
    $this_dir_list = scandir(getcwd());

    echo '<body>';
    echo '<ul>';

    echo '<li class="page_title">';
    echo "Neil Schroeder's Homepage";
    echo '</li>';

    foreach ($this_dir_list as $thisDir){
        if( is_dir($thisDir) ){
            if( !(in_array($thisDir, $except_dirs))){
                if( basename($thisDir) == "public_html" ){ 
                    //this is the initial row of directories, add a home button
                    echo '<li>';
                    echo '<a href=' . getBaseDirPrefix() . '/index.php> Home </a>';
                    echo '</li>';
                    if(basename(dirname(__DIR__)) != "public_html"){
                        echo '<li>';
                        echo '<a href=' . $thisDir . '/index.php> Back to '. basename(dirname(__DIR__)) .' </a>';
                        echo '</li>';
                    }
                } else {
                    if( $thisDir == ".."){
                        if( basename(getcwd()) != "public_html"){
                            echo '<li>';
                            echo '<a href=' . $thisDir . '/index.php>Back</a>';
                            echo '</li>';
                        }
                    }
                    else{
                        echo '<li>';
                        echo '<a href=' . $thisDir . '/index.php>' . $thisDir .'</a>';
                        echo '</li>';
                    }
                }
            }
        }
    }
    echo '</ul>';
    echo '</body>';
}

        //get the list of subsequent folders (assuming images do not not go with folders)
$dir_list = scandir( getcwd() );
$base_dir = '/home/schr1077/public_html'; // hard
$base_index_php = '/home/schr1077/public_html/resources/index.php'; // hard
$base_style_css = '/home/schr1077/public_html/resources/style.css'; // hard
$current_dir = getcwd();
$except_dir_list = array("resources", ".", "index.php", ".index.php.swp"); //directories (or specific files) you don't want listed on the page
$image_types_allowed = array("jpg","png","pdf"); //allowed imaged types
$dir_above = 'home/'.basename(dirname(__DIR__));

//link css by finding home directory 'public_html'
//assume a depth of less than 10, because that's too much
$base_dir_pre = getBaseDirPrefix();
echo '<link rel="stylesheet" href="'.$base_dir_pre.'resources/style.css">';
echo '</head>';

/***********************************************************************************/
/* PLOTS */

printUlLiFromArray( __DIR__, $except_dir_list);
echo '<body class="homepage">';

echo '<p class="location_statement">';
echo 'current directory: '.__DIR__;
echo '</p>';

//all the images go into a single row and the row is wrapped to the window size
echo '<div class="row">';
foreach( $dir_list as $thisImg){
    if( pathinfo($thisImg)['extension'] == "png"){
        echo '<div class="polaroid">';
        echo '<a href="'. $thisImg .'">';
        echo '<img src="'. $thisImg .'" alt="'. $thisImg .'">';
        echo '</a>';
        echo '<div class="caption">';
        //echo '<p class="polaroid_caption"> '. $thisImg .'</p>';
        echo '<p> '. $thisImg .'</p>';
        echo '</div>';
        echo '</div>';
    }
}
echo '</div>';
echo '</body>';

?>
 <!-- end php script -->

</html>
