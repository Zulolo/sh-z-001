var global_configuration;
$(function() {
	$('.error').hide();
	$.ajax({
		type: 'get',
		url: "global_config",
		context: this,
		success: function(data, status){
			$('#street_light_select').multiselect();
			alert("Data: " + data + "\nStatus: " + status);
	/* 		global_configuration = data;
			var options = [
				{label: 'Option 1', title: 'Option 1', value: '1', selected: true},
				{label: 'Option 2', title: 'Option 2', value: '2'},
				{label: 'Option 3', title: 'Option 3', value: '3', selected: true},
				{label: 'Option 4', title: 'Option 4', value: '4'},
				{label: 'Option 5', title: 'Option 5', value: '5'},
				{label: 'Option 6', title: 'Option 6', value: '6', disabled: true}
			];
			
			$('#street_light_select').multiselect('dataprovider', options); */
		},
		error: function(jqXHR, exception){
			alert("error");
			$('#street_light_select').multiselect();
		},
		cache: false,
	});
	$("#submit_time_setting").click(function() {
      // validate and process form here
	  
	  // post data using json format
    });
});