function validFiles(){
    $.ajax({ //This Gets all the valid files from uploads
        type: 'get',
        dataType: 'json',
        url: '/filesIn',
        data: {
            data1: "Value"
        },success: function (data) {
            $('#info1').html('<tr id="info"><th>file name (click to download)</th><th>Version</th><th>Creator</th><th>Number of waypoints</th><th>Number of routes</th><th>Number of tracks</th></tr>'); 
            $('#selector').html('<option value=\'0\'>Select GPX</option>');
            $('#selector4').html('<option value=\'0\'>Select GPX</option>');
            $('#selector5').html('<option value=\'0\'>Select GPX</option>');
            $('#selector8').html('<option value=\'0\'>Select GPX</option>');
            $('#selector11').html('<option value=\'0\'>Select GPX</option>');
            $('#selector13').html('<option value=\'0\'>Select GPX</option>');
            let arr = data.ret;
            if (arr.length == 0) { //This checks if the array is empty
                $('#info1').append('<tr id=\'info\'><td>No Files</td></tr>');
            } else {
                arr.forEach(arr => { //This goes through and adds valid files to the table
                    $('#info1').append('<tr id=\'info\'><td><a href=\''+arr.file+'\' download ">'+arr.file+'</a></td><td>'+arr.version+'</td><td>'+arr.creator+'</td><td>'+arr.numWaypoints+'</td><td>'+arr.numRoutes+'</td><td>'+arr.numTracks+'</td></tr>');
                    $('#selector').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                    $('#selector4').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                    $('#selector5').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                    $('#selector8').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                    $('#selector11').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                    $('#selector13').append('<option value=\''+arr.file+'\'>'+arr.file+'</option>');
                })
                $.ajax({ 
                    type: 'get',
                    dataType: 'json',
                    url: '/Store',
                    data: {
                        data1: 'value'
                    },success: function (data) {
                        let suc = data.ret;
                        if (suc == 1) {
                            $('#sucsess6').html("Login Success");
                            $.ajax({
                                type: 'get',
                                data: 'json',
                                url: '/printRoutesID',
                                data: {
                                    data1: "value"
                                },success: function (data){
                                    let blam = data.ret;
                                    if (blam != 0 || blam != "[]") {
                                        $('#selector10').html('<option value=\'0\'>Select Route</option>');
                                        for (let row2 of blam){
                                            $('#selector10').append('<option value=\''+row2.route_name+'\'>'+row2.route_name+'</option>');
                                        }
                                    } 
                                },fail: function(error){
                                    console.log(error); 
                                }
                            });
                        } else {
                            $('#sucsess6').html("Login failure");
                        }
                    },fail: function(error){
                        console.log(error); 
                    }
                });
            }
        },fail: function(error){
            console.log(error); 
        }
    }); //Complete
}

function updateGPXview(val) {
    if (val != 0) {
        $.ajax ({
            type: 'get',
            data: 'json',
            url: '/RouteAndTrack',
            data: {
                data1: val
            },success: function (data) {
                $('#info2').html('<tr id="info"><th>Component</th><th>Name</th><th>Number of points</th><th>Length</th><th>Loop</th></tr>');
                $('#selector2').html('<option value=\'0\'>Select Route or Track</option>');
                $('#selector3').html('<option value=\'0\'>Select Route or Track</option>');
                let arr = data.ret;
                if (arr.length == 0) { //This checks if the array is empty
                    $('#info2').append('<tr id=\'info\'><td>No Routes or Tracks</td></tr>');
                } else {
                    arr.forEach(arr => { //This goes through and adds valid files to the table
                        $('#info2').append('<tr id=\'info\'><td>'+arr.comp+'</td><td>'+arr.name+'</td><td>'+arr.numPoints+'</td><td>'+arr.len+'</td><td>'+arr.loop+'</td></tr>');
                        $('#selector2').append('<option value=\''+arr.comp+'\'>'+arr.comp+'</option>');
                        $('#selector3').append('<option value=\''+arr.comp+'\'>'+arr.comp+'</option>');
                    })
                }
            },fail: function(error){
                console.log(error); 
            }
        })
    }
}

$(document).ready(function() {

    //$('#content').toggle();

    $('#login').submit(function(e){
        e.preventDefault();
        let user = $('#loginUser').val();
        let pass = $('#loginPass').val();
        let serv = $('#loginServ').val();
        $.ajax({ 
            type: 'get',
            dataType: 'json',
            url: '/login',
            data: {
                data1: user,
                data2: pass,
                data3: serv
            },success: function (data) {
                let suc = data.ret;
                if (suc == 1) {
                    $('#content').toggle();
                    $('#Login').toggle();
                    validFiles();
                } else {
                    $('#sucsess6').html("Login failure");
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    })

    $('#Store').click(function(e){
        e.preventDefault();
        $.ajax({ 
            type: 'get',
            dataType: 'json',
            url: '/Store',
            data: {
                data1: 'value'
            },success: function (data) {
                let suc = data.ret;
                if (suc == 1) {
                    $('#sucsess7').html("Store success");
                    $.ajax({
                        type: 'get',
                        data: 'json',
                        url: '/printRoutesID',
                        data: {
                            data1: "value"
                        },success: function (data){
                            let blam = data.ret;
                            if (blam != 0 || blam != "[]") {
                                $('#selector10').html('<option value=\'0\'>Select Route</option>');
                                for (let row2 of blam){
                                    $('#selector10').append('<option value=\''+row2.route_name+'\'>'+row2.route_name+'</option>');
                                }
                            } 
                        },fail: function(error){
                            console.log(error); 
                        }
                    });
                } else {
                    $('#sucsess7').html("Store failure");
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    })

    $('#Clear').click(function(e){
        e.preventDefault();
        $.ajax({ 
            type: 'get',
            dataType: 'json',
            url: '/clear',
            data: {
                data1: 'value'
            },success: function (data) {
                let suc = data.ret; 
                if (suc == 1) {
                    $('#sucsess7').html("Clear success");
                } else {
                    $('#sucsess7').html("Clear failure");
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    })

    $('#Status').click(function(e){
        e.preventDefault();
        $.ajax({ 
            type: 'get',
            dataType: 'json',
            url: '/status',
            data: {
                data1: 'value'
            },success: function (data) {
                let suc = data.ret;
                if (suc == 0) {
                    $('#sucsess7').html("Status failure");
                } else {
                    $('#sucsess7').html('Database has '+suc.file+' files, '+suc.route+' routes, '+suc.point+' points.');
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    })

    $('#Routepoints').submit(function(e){
        e.preventDefault();
        let val = document.getElementById('selector10').value;
        if (val != 0) {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/printSpecPoints',
                data: {
                    data1: val
                },success: function (data){
                    let suc = data.ret;
                    if (suc == 0 || suc == "[]") {
                        $('#info8').html('<tr id="info"><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
                    } else {
                        $('#info8').html('<tr id="info"><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
                        for (let row of suc){
                            $('#info8').append('<tr id="info"><td>'+val+'</td><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td>'+row.point_name+'</td></tr>');
                        }
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        } else {
            $('#info8').html('<tr id="info"><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
        }
    })

    $('#Shortest').submit(function(e){
        e.preventDefault();
        let val = document.getElementById('selector13').value;
        let val1 = document.getElementById('selector14').value;
        let val2 = $('#entryBox14').val();
        if (val != 0 && val1 != 0 && val2 != "") {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/printSpecShort',
                data: {
                    data1: val,
                    data2: val1,
                    data3: val2
                },success: function (data){
                    let suc = data.ret;
                    if (suc == 0 || suc == "[]") {
                        $('#info10').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
                    } else {
                        $('#info10').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
                        for (let row of suc){
                            $('#info10').append('<tr id="info"><td>'+val+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td></tr>');
                        }
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        } else {
            $('#info10').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
        }
    })

    $('#FileRoutes').submit(function(e){
        e.preventDefault();
        let val = document.getElementById('selector8').value;
        let val1 = document.getElementById('selector9').value;
        if (val != 0 && val1 != 0) {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/printSpecRoutes',
                data: {
                    data1: val,
                    data2: val1
                },success: function (data){
                    let suc = data.ret;
                    if (suc == 0 || suc == "[]") {
                        $('#info7').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
                    } else {
                        $('#info7').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
                        for (let row of suc){
                            $('#info7').append('<tr id="info"><td>'+val+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td></tr>');
                        }
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        } else {
            $('#info7').html('<tr id="info"><th>File</th><th>Route ID</th><th>Route name</th><th>Route length</th></tr>');
        }
    })

    $('#FilePoints').submit(function(e){
        e.preventDefault();
        let val = document.getElementById('selector11').value;
        let val1 = document.getElementById('selector12').value;
        if (val != 0 && val1 != 0) {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/printSpecRoutesPoints',
                data: {
                    data1: val,
                    data2: val1
                },success: function (data){
                    let suc = data.ret;
                    if (suc == 0 || suc == "[]") {
                        $('#info9').html('<tr id="info"><th>File</th><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
                    } else {
                        $('#info9').html('<tr id="info"><th>File</th><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
                        for (let row of suc){
                            $('#info9').append('<tr id="info"><td>'+val+'</td><td>'+row.route_name+'</td><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td>'+row.point_name+'</td></tr>');
                        }
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        } else {
            $('#info9').html('<tr id="info"><th>File</th><th>Route name</th><th>Point ID</th><th>Point index</th><th>Latitude</th><th>Longitude</th><th>Point name</th></tr>');
        }
    })

    $('#Routes').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        let val = document.getElementById('selector7').value;
        if (val != 0) {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/printRoutes',
                data: {
                    data1: val
                },success: function (data){
                    let suc = data.ret;
                    if (suc == 0 || suc == "[]") {
                        $('#sucsess6').html("table failure");
                        $('#info6').html('<tr id="info"><th>Route ID</th><th>Route name</th><th>Route length</th><th>GPX ID</th></tr>');
                    } else {
                        $('#info6').html('<tr id="info"><th>Route ID</th><th>Route name</th><th>Route length</th><th>GPX ID</th></tr>');
                        for (let row of suc){
                            $('#info6').append('<tr id="info"><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');
                        }
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        } else {
            $('#info6').html('<tr id="info"><th>Route ID</th><th>Route name</th><th>Route length</th><th>GPX ID</th></tr>');
        }
    });

    $('#selector').change(function(){ //This gets the information of each route and track of the related 
        let val = document.getElementById('selector').value;
        $('#info2').html('<tr id=\'info\'><th>Component</th><th>Name</th><th>Number of points</th><th>Length</th><th>Loop</th></tr>');
        $('#info3').html('<tr id=\'info\'><th>Name</th><th>Value</th></tr>');
        $('#selector2').html('<option value=\'0\'>Select Route or Track</option>');
        $('#selector3').html('<option value=\'0\'>Select Route or Track</option>');
        if (val != 0) {
            updateGPXview(val);
        }
    }) //Complete

    $('#selector2').change(function(){ //This gets the other data of a specific route or track
        let val = document.getElementById('selector').value;
        let val1 = document.getElementById('selector2').value;
        $('#info3').html('<tr id=\'info\'><th>Name</th><th>Value</th></tr>');
        if (val != 0 && val1 != 0) {
            $.ajax ({
                type: 'get',
                data: 'json',
                url: '/DatatoJSON',
                data: {
                    data1: val,
                    data2: val1
                },success: function (data){
                    let json = data.somethingElse;
                    if (json != "[]") {
                        let arr = JSON.parse(json);
                        arr.forEach(arr => {
                            $('#info3').append('<tr id=\'info\'><td>'+arr.name+'</td><td>'+arr.value+'</td></tr>');
                        })
                    } else {
                        $('#info3').append('<tr id=\'info\'><td>No other data</td></tr>');
                    }
                },fail: function(error){
                    console.log(error); 
                }
            })
        }
    }) //Complete

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        let val = document.getElementById('selector').value;
        let val1 = document.getElementById('selector3').value;
        let val2 = $('#entryBox').val();
        $('#entryBox').val("");
        if (val != 0 && val1 != 0) {
            $.ajax({
                type: 'get',
                data: 'json',
                url: '/rename',
                data: {
                    data1: val,
                    data2: val1,
                    data3: val2
                },success: function (data){
                    let json = data.somethingElse;
                    if (json == "true") {
                        $('#sucsess').html("Rename was successful");
                        validFiles();
                        updateGPXview(val);
                    } else {
                        $('#sucsess').html("Rename failed");
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        }
    });

    $('#gpxform').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        let val = document.getElementById('selector').value;
        let val1 = $('#entryBox2').val();
        let val2 = $('#entryBox3').val();
        $('#entryBox2').val("");
        $('#entryBox3').val("");
        $.ajax({
            type: 'get',
            data: 'json',
            url: '/CreateGPX',
            data: {
                data1: val1,
                data2: val2
            },success: function (data){
                let json = data.somethingElse;
                if (json == "true") {
                    $('#sucsess2').html("Create successful");
                    validFiles();
                    if (val != 0) {
                        updateGPXview(val);
                    }
                } else {
                    $('#sucsess2').html("Create failed");
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    });

    $('#routeform').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        let val3 = document.getElementById('selector').value;
        let val = document.getElementById('selector4').value;
        let val2 = $('#entryBox4').val();
        let val1 = "{\"name\":\""+val2+"\"}";
        $('#entryBox4').val("");
        $.ajax({
            type: 'get',
            data: 'json',
            url: '/CreateRoute',
            data: {
                data1: val,
                data2: val1
            },success: function (data){
                let json = data.somethingElse;
                if (json == "true") {
                    $('#sucsess3').html("Create successful");
                    validFiles();
                    updateGPXview(val3);
                } else {
                    $('#sucsess3').html("Create failed");
                }
            },fail: function(error){
                console.log(error); 
            }
        });
    });

    $('#selector5').change(function(){
        $('#selector6').html('<option value=\'0\'>Select Route</option>');
        let val = document.getElementById('selector5').value;
        if (val != 0) {
            $.ajax ({
                type: 'get',
                data: 'json',
                url: '/RouteAndTrack',
                data: {
                    data1: val
                },success: function (data) {
                    let arr = data.ret;
                    if (arr.length == 0) { //This checks if the array is empty
                        $('#selector6').html('<option value=\'0\'>Select Route</option>');
                    } else {
                        arr.forEach(arr => { //This goes through and adds valid files to the table
                            if ((arr.comp).includes("Route") == true) {
                                $('#selector6').append('<option value=\''+arr.comp+'\'>'+arr.comp+'</option>');
                            }
                        })
                    }
                },fail: function(error){
                    console.log(error); 
                }
            })
        }
    })

    $('#wayPointform').submit(function(e){
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        let val5 = document.getElementById('selector').value
        let val = document.getElementById('selector5').value;
        let val1 = document.getElementById('selector6').value;
        let val3 = $('#entryBox5').val();
        let val4 = $('#entryBox6').val();
        let val2 = "{\"lat\":"+val3+",\"lon\":"+val4+"}";
        $('#entryBox5').val("");
        $('#entryBox6').val("");
        if ((val3 <= -90 || val3 >= 90) || (val4 <= -180 || val4 >= 180)) {
            $('#sucsess4').html("Create failed");
        } else {
            $('#sucsess4').html('');
            if (val != 0 && val1 != 0) {
                $.ajax({
                    type: 'get',
                    data: 'json',
                    url: '/CreateWaypoint',
                    data: {
                        data1: val,
                        data2: val1,
                        data3: val2
                    },success: function (data){
                        let json = data.somethingElse;
                        if (json == "true") {
                            $('#sucsess4').html("Create successful");
                            validFiles();
                            updateGPXview(val5);
                        } else {
                            $('#sucsess4').html("Create failed");
                        }
                    },fail: function(error){
                        console.log(error); 
                    }
                });
            }
        }
    });

    $('#Between').submit(function(e){
        e.preventDefault();
        let sourceLat = $('#entryBox7').val();
        let sourceLong = $('#entryBox8').val();
        let destLat = $('#entryBox9').val();
        let destLong = $('#entryBox10').val();
        let delta = $('#entryBox11').val();
        $('#entryBox7').val("");
        $('#entryBox8').val("");
        $('#entryBox9').val("");
        $('#entryBox10').val("");
        $('#entryBox11').val("");
        $('#info4').html('<tr id=\'info\'><th>File</th><th>Name</th><th>Number of points</th><th>Length</th><th>Loop</th></tr>');
        if ((sourceLat <= -90 || sourceLat >= 90) || (destLat <= -90 || destLat >= 90) || (sourceLong <= -180 || sourceLong >= 180) || (destLong <= -180 || destLong >= 180)) {
            $('#sucsess5').html('Failed because out of range');
        } else {
            $('#sucsess5').html('');
            $.ajax({
                type: 'get',
                dataType: 'json', 
                url: '/Between',
                data: {
                    data1: sourceLat,
                    data2: sourceLong,
                    data3: destLat,
                    data4: destLong,
                    data5: delta
                },success: function (data){
                    let arr = data.ret;
                    if (arr.length == 0) {
                        $('#info4').append('<tr id=\'info\'><td>No Files</td></tr>');
                    } else {
                        arr.forEach(arr => {
                            $('#info4').append('<tr id=\'info\'><td>'+arr.file+'</td><td>'+arr.name+'</td><td>'+arr.numPoints+'</td><td>'+arr.len+'</td><td>'+arr.loop+'</td></tr>');
                        })
                    }
                },fail: function(error){
                    console.log(error); 
                }
            });
        }
    });

    $('#Length').submit(function(e){
        e.preventDefault();
        var i = 0;
        var j = 0;
        let val = $('#entryBox12').val();
        let val1 = $('#entryBox13').val();
        $('#entryBox12').val("");
        $('#entryBox13').val("");
        $.ajax({
            type: 'get',
            dataType: 'json', 
            url: '/LengthPath',
            data: {
                data1: val,
                data2: val1
            },success: function (data){
                let arr = data.ret;
                $('#info5').html('<tr id=\'info\'><th>Components</th><th>Number</th></tr>');
                $('#info5').append('<tr id=\'info\'><td>Route</td><td>'+arr.Route+'</td></tr>');
                $('#info5').append('<tr id=\'info\'><td>Track</td><td>'+arr.Track+'</td></tr>');
            },fail: function(error){
                console.log(error); 
            }
        })
    });
});
