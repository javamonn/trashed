(function($) {
	var $window = $(window),
		$body = $('body'),
		$featured = $('#featured-slider'),
		$searchField = $('#search-field'),
		$searchResults = $('#search-results'),
		$searchCount = $('.results-count');

	$(document).ready(function() {
		// Search
		let ghostSearch = new JustGoodSearch({
			key: search_key,
			host: search_host,
			input: '#search-field',
			results: '#search-results',
			template: function(result) {
				let url = [location.protocol, '//', location.host].join('');
				let pubDate = new Date(result.published_at).toLocaleDateString(document.documentElement.lang, {year: "numeric",month: "long",day: "numeric"});
				return '<div class="result-item"><a href="' + url + '/' + result.slug + '/"><div class="result-title">' + result.title + '</div><div class="result-date">' + pubDate + '</div></a>';
			},
			options: {
				keys: [
					'title',
					'plaintext'
				],
			},
			api: {
				resource: 'posts',
				parameters: { 
					fields: ['title', 'slug', 'plaintext', 'published_at'],
					formats: 'plaintext',
				},
			},
			on: {
				afterDisplay: function(results){
					$searchCount.text(results.length);
				},
			}
		});

		// Hidden sections
		$('#show-sidebar, #hide-sidebar').on('click', function(e){
			$body.toggleClass('sidebar--opened');
			e.preventDefault();
		});
		$('#show-search, #hide-search').on('click', function(e){
			if ( $body.hasClass('search--opened') ) {
				$body.removeClass('search--opened');
				$searchField.val('');
				$searchResults.html('');
				$searchCount.text('0');
			} else {
				$body.addClass('search--opened');
				setTimeout(function() {
					$searchField.focus();
				}, 300);
			}
			e.preventDefault();
		});
		$('#site-overlay').on('click', function(e){
			$body.removeClass('sidebar--opened search--opened');
			$searchField.val('');
			$searchResults.html('');
			$searchCount.text('0');
			e.preventDefault();
		});

		// Featured carousel
		$featured.slick({
			autoplay: true,
			arrows : true,
			dots : false,
			fade : true,
			appendArrows : $('.featured-nav'),
			prevArrow : $('.featured-prev'),
			nextArrow : $('.featured-next')
		});
		$featured.fadeIn(600, function(){
			$featured.parents().removeClass('slider-loading');
		});

		// Back to top button
		$('#top-link').on('click', function(e) {
			$('html, body').animate({'scrollTop': 0});
			e.preventDefault();
		});
		$window.scroll(function () {
			if ( $(this).scrollTop() > 600 ) {
				$body.addClass('is--scrolled');
			} else {
				$body.removeClass('is--scrolled');
			}
		});

		// Responsive videos
		$('.post').fitVids();

		// Image adjustments
		if ( $body.hasClass( 'post-template' ) || $body.hasClass( 'page-template' ) ) {
			adjustImages();
		}

		// Gallery adjustments
		$( '.kg-gallery-image > img' ).each( function() {
			var _this = $(this),
				$container = _this.closest('.kg-gallery-image'),
				width = _this.attr('width'),
				height = _this.attr('height'),
				ratio = width / height;
			$container.css({'flex' : ratio + ' 1 0%' });
		});

		// Grid layout
		if ( $.isFunction( $.fn.masonry ) && $('#post-wrapper').length ) {
			gridLayout.refresh();
		}

		// Instagram feed
		if ( typeof instagram_user_id !== 'undefined' && typeof instagram_access_token !== 'undefined' ) {
			if ( $('#instafeed').length ) {
				var userFeed = new Instafeed({
					get: 'user',
					userId: instagram_user_id,
					accessToken: instagram_access_token,
					limit: 6,
					resolution: 'low_resolution',
					template: '<div class="instagram-item"><a target="_blank" href="{{link}}"><img src="{{image}}" alt="{{caption}}" /></a></div>'
				});
				userFeed.run();
			}
		}

	});

	$window.on('debouncedresize', onResize);

	var gridLayout = (function() {
		var $container = $('#post-wrapper'),
			$items = $container.children().addClass('post--loaded'),
			initialized = false,
			init = function() {
				$container.imagesLoaded(function() {
					$container.masonry({
						itemSelector: '.post',
						columnWidth: '.post',
						transitionDuration: 0
					});
					setTimeout(function() {
						$container.masonry('layout');
					}, 100);
					showItems($items);
					initialized = true;
				});
			},
			refresh = function() {
				if (!initialized) {
					init();
					return;
				}
				$container.masonry('layout');
			},
			showItems = function($items) {
				$items.each(function(i, obj) {
					var $postInside = $(obj).find('.post-inside');
					animatePost($postInside, i * 100);
				});
			},
			animatePost = function($postInside, delay) {
				setTimeout(function() {
					$postInside.addClass('is--visible');
				}, delay);
			};
		return {
			init: init,
			refresh: refresh
		}
	})();

	function onResize() {
		if ( $body.hasClass( 'post-template' ) || $body.hasClass( 'page-template' ) ) {
			adjustImages();
		}
		if ( $.isFunction( $.fn.masonry ) && $('#post-wrapper').length ) {
			gridLayout.refresh();
		}
	}

	function adjustImages() {
		var $entry = $('.post'),
			$entryContent = $entry.find('.entry-content'),
			entryWidth = $entry.width(),
			entryContentWidth = $entryContent.width(),
			margin = entryContentWidth / 2 - entryWidth / 2;

		$entryContent.find('.alignleft').each(function() {
			$(this).css({ 'margin-left': margin });
		});
		$entryContent.find('.alignright').each(function() {
			var _this = $(this),
				elName = _this.prop('tagName').toLowerCase();
			if ( elName == 'blockquote' ) {
				_this.css({ 'margin-right': margin + 15 });
			} else {
				_this.css({ 'margin-right': margin });
			}
		});
	}

})(jQuery);
