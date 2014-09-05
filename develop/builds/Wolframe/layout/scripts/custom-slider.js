jQuery(document).ready(function ($) {

	/* Slider */
	$("#rslides").responsiveSlides({
		auto: true, // Boolean: Animate automatically, true or false
		speed: 300, // Integer: Speed of the transition, in milliseconds
		timeout: 4000, // Integer: Time between slide transitions, in milliseconds
		pager: true, // Boolean: Show pager, true or false
		nav: false, // Boolean: Show navigation, true or false
		random: false, // Boolean: Randomize the order of the slides, true or false
		pause: true, // Boolean: Pause on hover, true or false
		pauseControls: true, // Boolean: Pause when hovering controls, true or false
		prevText: "Previous", // String: Text for the "previous" button
		nextText: "Next", // String: Text for the "next" button
		maxwidth: "", // Integer: Max-width of the slideshow, in pixels
		navContainer: "", // Selector: Where controls should be appended to, default is after the 'ul'
		manualControls: "", // Selector: Declare custom pager navigation
		namespace: "centered-btns" // String: Change the default namespace used
	});

});
