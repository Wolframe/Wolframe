jQuery(document).ready(function ($) {

	/* Alert Messages */
	$(".alert-msg .close").click(function () {
		$(this).parent().animate({"opacity": "0"}, 400).slideUp(400);
		return false;
	});

	/* Accordions */
	$(".accordion-title").click(function () {
		$(".accordion-title").removeClass("active");
		$(".accordion-content").slideUp("normal");
		if ($(this).next().is(":hidden") == true) {
			$(this).addClass("active");
			$(this).next().slideDown("normal");
		}
	});
	$(".accordion-content").hide();

	/* Toggles */
	$(".toggle-title").click(function () {
		$(this).toggleClass("active").next().slideToggle("fast");
		return false;
	});

	/* Tabs */
	$(".tab-wrapper").tabs({
		event: "click"
	})

	/* Vertically Centre Text On Images */
	$.fn.flexVerticalCenter = function (onAttribute) {
		return this.each(function () {
			var $this = $(this);
			var attribute = onAttribute || 'margin-top';
			var resizer = function () {
				$this.css(
				attribute, (($this.parent().height() - $this.height()) / 2));
			};
			resizer();
			$(window).resize(resizer);
		});
	};

	// To run the function:
	$('.viewit').flexVerticalCenter();

});
