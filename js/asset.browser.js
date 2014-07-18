(function($) {

Drupal.behaviors.asset_browser = {
  attach: function (context, settings) {
    var self = this;
    $('.asset-inline-entity-browser .asset-select', context).once().click( self.assetSelect );
  },

  assetSelect: function ( e ) {
    e.preventDefault();
    var self = this;
    var $this = $(this);
    var id = $this.attr('data-id');
    var $wrapper = $this.closest('.asset-inline-entity-browser');
    var parents = $('.parents',$wrapper).val();
    console.log($wrapper);
    $('.asset-inline-entity',$wrapper).val('Asset (' + id + ')');
    $('input.ief-entity-submit,button.ief-entity-submit',$wrapper).trigger('mousedown');
  }
}

})(jQuery);