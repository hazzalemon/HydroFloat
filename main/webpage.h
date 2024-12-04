/*


  OK, ya ready for some fun? HTML + CSS styling + javascript all in and undebuggable environment

  one trick I've learned to how to debug HTML and CSS code.

  get all your HTML code (from html to /html) and past it into this test site
  muck with the HTML and CSS code until it's what you want
  https://www.w3schools.com/html/tryit.asp?filename=tryhtml_intro

  No clue how to debug javascrip other that write, compile, upload, refresh, guess, repeat

  I'm using class designators to set styles and id's for data updating
  for example:
  the CSS class .tabledata defines with the cell will look like
  <td><div class="tabledata" id = "switch"></div></td>

  the XML code will update the data where id = "switch"
  java script then uses getElementById
  document.getElementById("switch").innerHTML="Switch is OFF";


  .. now you can have the class define the look AND the class update the content, but you will then need
  a class for every data field that must be updated, here's what that will look like
  <td><div class="switch"></div></td>

  the XML code will update the data where class = "switch"
  java script then uses getElementsByClassName
  document.getElementsByClassName("switch")[0].style.color=text_color;


  the main general sections of a web page are the following and used here

  <html>
    <style>
    // dump CSS style stuff in here
    </style>
    <body>
      <header>
      // put header code for cute banners here
      </header>
      <main>
      // the buld of your web page contents
      </main>
      <footer>
      // put cute footer (c) 2021 xyz inc type thing
      </footer>
    </body>
    <script>
    // you java code between these tags
    </script>
  </html>


*/

// note R"KEYWORD( html page code )KEYWORD"; 
// again I hate strings, so char is it and this method let's us write naturally

const char PAGE_MAIN[] = R"=====(

<!DOCTYPE html>
<html lang="en" class="js-focus-visible">

<title>HydroFloat setup page</title>

  <style>
    table {
      position: relative;
      width:100%;
      border-spacing: 0px;
    }
    tr {
      border: 1px solid white;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
    }
    th {
      height: 20px;
      padding: 3px 15px;
      background-color: #343a40;
      color: #FFFFFF !important;
      }
    td {
      height: 20px;
       padding: 3px 15px;
    }
    .tabledata {
      font-size: 24px;
      position: relative;
      padding-left: 5px;
      padding-top: 5px;
      height:   25px;
      border-radius: 5px;
      color: #FFFFFF;
      line-height: 20px;
      transition: all 200ms ease-in-out;
      background-color: #00AA00;
    }
    .bodytext {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 24px;
      text-align: left;
      font-weight: light;
      border-radius: 5px;
      display:inline;
    }
    .navbar {
      width: 100%;
      height: 50px;
      margin: 0;
      padding: 10px 0px;
      background-color: #FFF;
      color: #000000;
      border-bottom: 5px solid #293578;
    }
    .fixed-top {
      position: fixed;
      top: 0;
      right: 0;
      left: 0;
      z-index: 1030;
    }
    .navtitle {
      float: left;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 50px;
      font-weight: bold;
      line-height: 50px;
      padding-left: 20px;
    }
   .navheading {
     position: fixed;
     left: 60%;
     height: 50px;
     font-family: "Verdana", "Arial", sans-serif;
     font-size: 20px;
     font-weight: bold;
     line-height: 20px;
     padding-right: 20px;
   }
   .navdata {
      justify-content: flex-end;
      position: fixed;
      left: 70%;
      height: 50px;
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      font-weight: bold;
      line-height: 20px;
      padding-right: 20px;
   }
    .category {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: bold;
      font-size: 32px;
      line-height: 50px;
      padding: 20px 10px 0px 10px;
      color: #000000;
    }
    .heading {
      font-family: "Verdana", "Arial", sans-serif;
      font-weight: normal;
      font-size: 28px;
      text-align: left;
    }
  
    .btn {
      background-color: #444444;
      border: none;
      color: white;
      padding: 10px 20px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 4px 2px;
      cursor: pointer;
    }
    .foot {
      font-family: "Verdana", "Arial", sans-serif;
      font-size: 20px;
      position: relative;
      height:   30px;
      text-align: center;   
      color: #AAAAAA;
      line-height: 20px;
    }
    .container {
      max-width: 1800px;
      margin: 0 auto;
    }
    table tr:first-child th:first-child {
      border-top-left-radius: 5px;
    }
    table tr:first-child th:last-child {
      border-top-right-radius: 5px;
    }
    table tr:last-child td:first-child {
      border-bottom-left-radius: 5px;
    }
    table tr:last-child td:last-child {
      border-bottom-right-radius: 5px;
    }
    
  </style>

  <body style="background-color: #efefef" onload="process()">
  
    <header>
      <div class="navbar fixed-top">
          <div class="container">
            <div class="navtitle">HydroFloat</div>
            <div class="navdata" id = "date">mm/dd/yyyy</div>
            <div class="navheading">DATE</div><br>
            <div class="navdata" id = "time">00:00:00</div>
            <div class="navheading">TIME</div>
            
          </div>
      </div>
    </header>
  
    <main class="container" style="margin-top:70px">
      <div class="category">Sensor Readings</div>
      <div style="border-radius: 10px !important;">
            <table style="width:50%">
      <colgroup>
        <col span="1" style="background-color:rgb(230,230,230); width: 20%; color:#000000 ;">
        <col span="1" style="background-color:rgb(200,200,200); width: 15%; color:#000000 ;">
      </colgroup>
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <col span="2"style="background-color:rgb(0,0,0); color:#FFFFFF">
      <tr>
        <th colspan="1"><div class="heading">Pin</div></th>
        <th colspan="1"><div class="heading"></div></th>
      </tr>
      <tr>
        <td><div class="bodytext">Calibrated Density</div></td>
        <td><div class="tabledata" id = "density"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">Float angle</div></td>
        <td><div class="tabledata" id = "float_ang"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">Temperature</div></td>
        <td><div class="tabledata" id = "tempr"></div></td>
      </tr>
      <tr>
        <td><div class="bodytext">Battery Voltage</div></td>
        <td><div class="tabledata" id = "bat"></div></td>
      </tr>
        <tr>
        <td><div class="bodytext">Digital switch</div></td>
        <td><div class="tabledata" id = "switch"></div></td>
      </tr>
      </table>
    </div>
    <br>
    <div class="category">Sensor Controls</div>
    <br>
    <input type="text" id="x4" name="x4">
    <label for="x1">x^4 + </label>
    <input type="text" id="x3" name="x3">
    <label for="lname">x^3 + </label>
    <input type="text" id="x2" name="x2">
    <label for="lname">x^2 + </label>
    <input type="text" id="x1" name="x1">
    <label for="lname">x + </label>
    <input type="text" id="b" name="b">
    <button type="button" class = "btn" id = "send_cal" onclick="Submit_Cal()">Submit Calibration</button>
    </form>
    
    </div>
    <br>
    <input type="text" id="SSID" name="SSID">
    <label for="SSID">WIFI SSID </label>
    <input type="text" id="Password" name="Password">
    <label for="Password">WIFI Password </label>
    <button type="button" class = "btn" id = "send_wifi" onclick="Update_WIFI()">Update WIFI</button>
    </div>
    <br>
    <br>
    <button type="button" class = "btn" id = "enter_MQTT" onclick="enter_MQTT()">enter MQTT</button>
    </div>
    <br>
    <br>
  </main>
  <div id="OTA">
    <h2>ESP32 Firmware Update</h2>
      <label id="latest_firmware_label">Latest Firmware: </label>
      <div id="latest_firmware"></div> 
      <input type="file" id="selected_file" accept=".bin" style="display: none;" onchange="getFileInfo()" />
      <div class="buttons">
        <input type="button" value="Select File" onclick="document.getElementById('selected_file').click();" />
        <input type="button" value="Update Firmware" onclick="updateFirmware()" />
      </div>
      <h4 id="file_info"></h4>
      <h4 id="ota_update_status"></h4>
    </div>
    <hr>

  <footer div class="foot" id = "temp" >Hydrofloat websever v1.69</div></footer>
  
  </body>


  <script type = "text/javascript">
  
    // global variable visible to all java functions
    var xmlHttp=createXmlHttpObject();

    // function to create XML object
    function createXmlHttpObject(){
      if(window.XMLHttpRequest){
        xmlHttp=new XMLHttpRequest();
      }
      else{
        xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
      }
      return xmlHttp;
    }

    function enter_MQTT(){
    var xhttp = new XMLHttpRequest(); 
    xhttp.open("PUT","enterMQTT", true);
    xhttp.send();
    }
    
    function Update_WIFI(){
      var xhttp = new XMLHttpRequest(); 
      var message;
      var ssid = document.getElementById("SSID").value;
      var pass = document.getElementById("Password").value;
      // var packet = ("wifi?VALUE=${ssid} ");
      xhttp.open("PUT","wifi?" + ssid + "?" + pass + "?", true);
      xhttp.send();
    }

    function Submit_Cal() {
      var xhttp = new XMLHttpRequest(); 
      var message;
      var coef4 = document.getElementById("x4").value;
      var coef3 = document.getElementById("x3").value;
      var coef2 = document.getElementById("x2").value;
      var coef1 = document.getElementById("x1").value;
      var coefb = document.getElementById("b").value;
      xhttp.open("PUT","coefs?" + coef4 + "?" + coef3 + "?"+ coef2 + "?" + coef1 + "?" + coefb, true);
      xhttp.send();
      // if you want to handle an immediate reply (like status from the ESP
      // handling of the button press use this code
      // since this button status from the ESP is in the main XML code
      // we don't need this
      // remember that if you want immediate processing feedbac you must send it
      // in the ESP handling function and here
      /*
      xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          message = this.responseText;
          // update some HTML data
        }
      }
      */
    }

    // function to handle the response from the ESP
    function response(){
      var message;
      var barwidth;
      var currentsensor;
      var xmlResponse;
      var xmldoc;
      var dt = new Date();
      var color = "#e8e8e8";
     
      // get the xml stream
      xmlResponse=xmlHttp.responseXML;
  
      // get host date and time
      document.getElementById("time").innerHTML = dt.toLocaleTimeString();
      document.getElementById("date").innerHTML = dt.toLocaleDateString();
  
      // A0
      xmldoc = xmlResponse.getElementsByTagName("Float_ang"); //bits for A0
      message = xmldoc[0].firstChild.nodeValue;
      
      if (message > 2048){
      color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      
      barwidth = message / 40.95;
      document.getElementById("float_ang").innerHTML=message;
      document.getElementById("float_ang").style.width=(barwidth+"%");
      // if you want to use global color set above in <style> section
      // other wise uncomment and let the value dictate the color
      //document.getElementById("float_ang").style.backgroundColor=color;
      //document.getElementById("float_ang").style.borderRadius="5px";
      
      xmldoc = xmlResponse.getElementsByTagName("density"); 
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("density").innerHTML=message;
      document.getElementById("density").style.width=(barwidth+"%");
      // you can set color dynamically, maybe blue below a value, red above
      document.getElementById("density").style.backgroundColor=color;
      //document.getElementById("density").style.borderRadius="5px";

      xmldoc = xmlResponse.getElementsByTagName("tempr"); 
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("tempr").innerHTML=message;
      document.getElementById("tempr").style.width=(barwidth+"%");
      // you can set color dynamically, maybe blue below a value, red above
      document.getElementById("tempr").style.backgroundColor=color;
      //document.getElementById("tempr").style.borderRadius="5px";
  

      // A1
      xmldoc = xmlResponse.getElementsByTagName("bat");
      message = xmldoc[0].firstChild.nodeValue;
      if (message > 4){
      color = "#aa0000";
      }
      else {
        color = "#0000aa";
      }
      document.getElementById("bat").innerHTML=message;
      width = message / 4.2;
      document.getElementById("bat").style.width=(width+"%");
      document.getElementById("bat").style.backgroundColor=color;
      //document.getElementById("bat").style.borderRadius="5px";
      
      xmldoc = xmlResponse.getElementsByTagName("ssid");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("SSID").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("wifi_password");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("Password").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("coef4");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("x4").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("coef3");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("x3").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("coef2");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("x2").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("coef1");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("x1").placeholder=message;

      xmldoc = xmlResponse.getElementsByTagName("coefb");
      message = xmldoc[0].firstChild.nodeValue;
      document.getElementById("b").placeholder=message;
  
      xmldoc = xmlResponse.getElementsByTagName("LED");
      message = xmldoc[0].firstChild.nodeValue;
    }
    // general processing code for the web page to ask for an XML steam
    // this is actually why you need to keep sending data to the page to 
    // force this call with stuff like this
    // server.on("/xml", SendXML);
    // otherwise the page will not request XML from the ESP, and updates will not happen
    function process(){
     
     if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
        xmlHttp.open("PUT","xml",true);
        xmlHttp.onreadystatechange=response;
        xmlHttp.send(null);
      }       
        // you may have to play with this value, big pages need more porcessing time, and hence
        // a longer timeout
        setTimeout("process()",200);
    }

    function updateFirmware() 
    {
      // Form Data
      var formData = new FormData();
      var fileSelect = document.getElementById("selected_file");

      if (fileSelect.files && fileSelect.files.length == 1) 
      {
        var file = fileSelect.files[0];
        formData.set("file", file, file.name);
        document.getElementById("ota_update_status").innerHTML = "Uploading " + file.name + ", Firmware Update in Progress...";

        // Http Request
        var request = new XMLHttpRequest();

        // request.upload.addEventListener("progress", updateProgress);
        request.open('POST', "/OTAupdate");
        request.responseType = "blob";
        request.send(formData);
      }
      else 
      {
        window.alert('Select A File First')
      }
    }
  
  </script>

</html>



)=====";