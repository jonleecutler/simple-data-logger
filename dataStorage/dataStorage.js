var filePath = '../dataBase.csv'; // Store the data in a folder above

var serialPort = require('serialport'); // serial library
var readLine = serialPort.parsers.Readline; // read serial data as lines
var  fs = require('fs'); // Filesystem access to write the data
var  fd; // file identifier



fs.open(filePath, 'a+', function (err, file) { // opening the file in appending mode
  if (err) throw err;
  fd=file;
  console.log("Opened file");
  fs.fstat(fd,function (err, stat) {
    if (err) throw err;
    if(stat.size==0){// if the file is empty, we write the first 'key' line
      fs.appendFile(fd, "date,value,note,beats\r\n", function (err) {
        if (err) throw err;
        console.log("File was empty, added key line to csv.");
     });
    }
  });
});





//---------------------- SERIAL COMMUNICATION --------------------------------//
// start the serial port connection and read on newlines
// '/dev/cu.SLAB_USBtoUART'
const serial = new serialPort('/dev/ttyUSB0', {
 baudRate:115200

});
const parser = new readLine({
  delimiter: '\r\n'
});
// Read data that is available on the serial port and send it to the websocket

serial.pipe(parser);
parser.on('data', function(data) {
  var newEntry = ((Date.now()) + ',' + data + '\r\n'); //connect the data to the current time
  fs.appendFile(fd, newEntry, function (err) { // append the new entry to the file
    if (err) throw err;
  });
});
//----------------------------------------------------------------------------//
