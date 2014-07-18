<?php

/**
 * UI controller.
 */
class AssetUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults. Main reason for doing this is that
   * parent class hook_menu() is optimized for entity type administration.
   */
  public function hook_menu() {

    $items = array();
    $id_count = count(explode('/', $this->path));
    $wildcard = isset($this->entityInfo['admin ui']['menu wildcard']) ? $this->entityInfo['admin ui']['menu wildcard'] : '%' . $this->entityType;

    $items[$this->path] = array(
      'title' => 'Assets',
      'description' => 'Add edit and update assets.',
      'page callback' => 'system_admin_menu_block_page',
      'access arguments' => array('administer asset types'),
      'file path' => drupal_get_path('module', 'system'),
      'file' => 'system.admin.inc',
    );

    // Change the overview menu type for the list of assets.
    $items[$this->path]['type'] = MENU_LOCAL_TASK;

    // Change the add page menu to multiple types of entities
    $items[$this->path . '/add'] = array(
      'title' => 'Add an asset',
      'description' => 'Add a new asset',
      'page callback'  => 'asset_add_page',
      'access callback'  => 'asset_access',
      'access arguments' => array('edit'),
      'type' => MENU_NORMAL_ITEM,
      'weight' => 20,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );

    // Add menu items to add each different type of entity.
    foreach (asset_get_types() as $type) {
      $items[$this->path . '/add/' . $type->type] = array(
        'title' => 'Add ' . $type->name,
        'page callback' => 'asset_form_wrapper',
        'page arguments' => array($type->type),
        'access callback' => 'asset_access',
        'access arguments' => array('edit', $type),
        'file' => 'asset.admin.inc',
        'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
      );
    }

    $items[$this->path . '/manage/' . $wildcard] = array(
      'title callback' => 'asset_form_title',
      'title arguments' => array($id_count + 1),
      'page callback' => 'asset_form_wrapper',
      'page arguments' => array($id_count + 1),
      'access callback' => 'asset_access',
      'access arguments' => array('edit', $id_count + 1),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );
    $items[$this->path . '/manage/' . $wildcard . '/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );

    $items[$this->path . '/manage/' . $wildcard . '/delete'] = array(
      'title' => 'Delete',
      'page callback' => 'drupal_get_form',
      'page arguments' => array('asset_delete_form', $id_count + 1, $id_count + 2),
      'access callback' => 'asset_access',
      'access arguments' => array('delete', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 10,
      'file' => 'asset.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );

    $items[$this->path . '/manage/' . $wildcard . '/instance'] = array(
      'title' => 'Add Instance',
      'title callback' => 'asset_instance_create_form_title',
      'title arguments' => array($id_count + 1),
      'page callback' => 'asset_instance_form_wrapper',
      'page arguments' => array($id_count + 1, $id_count + 2),
      'access callback' => 'asset_access',
      'access arguments' => array('instance', $id_count + 1),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'weight' => -10,
      'file' => 'assetInstance.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );

    $items[$this->path . '/manage/' . $wildcard . '/instance/%asset_instance'] = array(
      'title callback' => 'asset_instance_form_title',
      'title arguments' => array($id_count + 1, $id_count + 3),
      'page callback' => 'asset_instance_form_wrapper',
      'page arguments' => array($id_count + 1, $id_count + 3),
      'access callback' => 'asset_instance_access',
      'access arguments' => array('edit', $id_count + 3),
      'weight' => 0,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
      'file' => 'assetInstance.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );
    $items[$this->path . '/manage/' . $wildcard . '/instance/%asset_instance/edit'] = array(
      'title' => 'Edit',
      'type' => MENU_DEFAULT_LOCAL_TASK,
      'weight' => -10,
      'context' => MENU_CONTEXT_PAGE | MENU_CONTEXT_INLINE,
    );

    $items[$this->path . '/manage/' . $wildcard . '/instance/%asset_instance/delete'] = array(
      'title' => 'Delete',
      'title callback' => 'asset_instance_form_title',
      'title arguments' => array($id_count + 1, $id_count + 3, $id_count + 4),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('asset_instance_delete_form', $id_count + 1, $id_count + 3),
      'access callback' => 'asset_instance_access',
      'access arguments' => array('delete', $id_count + 3),
      'type' => MENU_LOCAL_TASK,
      'context' => MENU_CONTEXT_INLINE,
      'weight' => 0,
      'file' => 'assetInstance.admin.inc',
      'file path' => drupal_get_path('module', $this->entityInfo['module']) . '/includes'
    );

    // Menu item for viewing assets
    $items['asset/' . $wildcard] = array(
      //'title' => 'Title',
      'title callback' => 'asset_page_title',
      'title arguments' => array(1),
      'page callback' => 'asset_page_view',
      'page arguments' => array(1),
      'access callback' => 'asset_access',
      'access arguments' => array('view', 1),
      'type' => MENU_CALLBACK,
    );

    return $items;
  }

  /**
   * Create the markup for the add Asset Entities page within the class
   * so it can easily be extended/overriden.
   */
  public function addPage() {
    $item = menu_get_item();
    $content = system_admin_menu_block($item);

    if (count($content) == 1) {
      $item = array_shift($content);
      drupal_goto($item['href']);
    }

    return theme('asset_add_list', array('content' => $content));
  }

}

/**
 * Title for management pages.
 */
function asset_form_title(Asset $asset, $op = 'edit'){
  return ucfirst($op) . ' ' . $asset->name;
}

/**
 * Page to add Asset Entities.
 *
 * @todo Pass this through a proper theme function
 */
function asset_add_page() {
  $controller = entity_ui_controller('asset');
  return $controller->addPage();
}

/**
 * Form callback wrapper: create or edit a asset.
 *
 * @param $asset
 *   The asset object being edited by this form.
 *
 * @see asset_edit_form()
 */
function asset_form_wrapper($asset) {
  if(!is_a($asset, 'Asset')){
    $asset = asset_create(array('type' => $asset));
  }
  // Add the breadcrumb for the form's location.
  asset_set_breadcrumb();
  return drupal_get_form('asset_edit_form', $asset);
}

/**
 * Form callback: create or edit a asset.
 *
 * @param $asset
 *   The asset object to edit or for a create form an empty asset object
 *     with only a asset type defined.
 */
function asset_edit_form($form, &$form_state, $asset) {
  $asset_type = $form_state['asset_type'] = asset_type_load($asset->type);
  $instance = $asset->instance;
  $form_state['asset'] = $asset;
  $form_state['instance'] = $instance;

  // Add the default field elements.
  if(empty($asset_type->data['auto_title'])){
    $form['name'] = array(
      '#type'          => 'textfield',
      '#title'         => t($asset_type->name . ' Name'),
      '#default_value' => isset($asset->name) ? $asset->name : '',
      '#maxlength'     => 255,
      '#required'      => TRUE,
      '#weight'        => -5,
    );
  }

  $form['data']['#tree'] = TRUE;

  // Add the field related form elements.
  field_attach_form('asset', $asset, $form, $form_state);

  $form['instance_form'] = array(
    '#weight' => 200,
    '#access' => FALSE,
    '#tree' => TRUE,
  );
  $form['instance_form']['status'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Set default options'),
    '#default_value' => !empty($asset->instance_id) ? 1 : 0,
  );

  $form['instance_form']['data'] = array(
    '#type' => 'fieldset',
    '#tree' => TRUE,
    '#title' => t('Instance Settings')
  );

  if(!empty($asset_type->data['require_instance'])){
    // If instances are required for this asset type, disable the option
    // to turn them off.
    $form['instance_form']['status'] = array(
      '#type' => 'value',
      '#value' => 1,
    );
    $form['instance_form']['data']['#type'] = 'item';
    $form['instance_form']['data']['#title'] = NULL;
  }

  if($instance_fields = $instance->widgetFields()){
    $form['instance_form']['#access'] = TRUE;
    $form['instance_form']['data']['#states'] = array(
      'visible' => array(
        ':input[name="instance_form[status]"]' => array('checked' => TRUE),
      ),
    );
    $form['instance_form']['data'] += $instance_fields;
  }

  $form['actions'] = array(
    '#type'       => 'container',
    '#attributes' => array('class' => array('form-actions')),
    '#weight'     => 400,
  );

  // We add the form's #submit array to this button along with the actual submit
  // handler to preserve any submit handlers added by a form callback_wrapper.
  $submit = array();
  if (!empty($form['#submit'])) {
    $submit += $form['#submit'];
  }

  $form['actions']['submit'] = array(
    '#type'   => 'submit',
    '#value'  => t('Save') . ' ' . $asset_type->name,
    '#submit' => $submit + array('asset_edit_form_submit'),
  );

  if (!empty($asset->id) && empty($asset_type->data['purge'])) {
    $title = 'Delete';
    if(function_exists('fett_icon')) fett_icon($title);
    $form['actions']['delete']['#markup'] = l($title, 'admin/content/assets/manage/'.$asset->id.'/delete', array('html'=>TRUE, 'attributes'=>array('class'=>array('button','tiny','text'))));
  }

  // We append the validate handler to #validate in case a form callback_wrapper
  // is used to add validate handlers earlier.
  $form['#validate'][] = 'asset_edit_form_validate';
  return $form;
}

/**
 * Form API validate callback for the asset form
 */
function asset_edit_form_validate($form, &$form_state) {
  $values = $form_state['values'];
  $asset = (object) $form_state['asset'];
  // Notify field widgets to validate their data.
  field_attach_form_validate('asset', $asset, $form, $form_state);

  if(!empty($values['instance_form']['status'])){
    $instance = $asset->instance;
    $instance->widgetFieldsValidate($values['instance_form']['data']);
  }
}


/**
 * Form API submit callback for the asset form.
 *
 * @todo remove hard-coded link
 */
function asset_edit_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $asset = (object) $form_state['asset'];
  $asset = entity_ui_controller('asset')->entityFormSubmitBuildEntity($form, $form_state);
  $asset->save();
  $form_state['redirect'] = 'asset/' . $asset->id;
}

/**
 * Form callback: confirmation form for deleting a asset.
 *
 * @param $asset
 *   The asset to delete
 *
 * @see confirm_form()
 */
function asset_delete_form($form, &$form_state, $asset) {
  $form_state['asset'] = $asset;
  $usage = asset_usage($asset->id);

    $vars['content']['usage'] = array(
      '#theme' => 'asset_usage',
      '#usage' => $usage,
      '#weight' => 100,
    );

  $form['#submit'][] = 'asset_delete_form_submit';

  $message = '<p>'.t('This action cannot be undone.').'</p>';
  if($usage = asset_usage($asset->id)){
    $form['#attached']['css'][] = drupal_get_path('module', 'asset') . '/css/asset.full.scss';

    $message = '<div class="alert-box alert"><h4>'.t('This action cannot be undone.').'</h4><strong>'.t('This asset is currently being used.').'</strong> '.t('Deleting this asset remove it from anywhere it is used.').'</div>';

    $form['usage'] = array(
      '#theme' => 'asset_usage',
      '#usage' => $usage,
      '#weight' => 1,
    );
  }

  $form = confirm_form($form,
    t('Are you sure you want to delete asset %name?', array('%name' => $asset->name)),
    'admin/content/assets/asset',
    $message,
    t('Delete'),
    t('Cancel'),
    'confirm'
  );

  return $form;
}

/**
 * Submit callback for asset_delete_form
 */
function asset_delete_form_submit($form, &$form_state) {
  $asset = $form_state['asset'];
  $asset->delete();
  drupal_set_message(t('The asset %name has been deleted.', array('%name' => $asset->name)));
  watchdog('asset', 'Deleted asset %name.', array('%name' => $asset->name));

  $form_state['redirect'] = 'admin/content/assets';
}

/**
 * Sets the breadcrumb for administrative asset pages.
 */
function asset_set_breadcrumb() {
  $breadcrumb = array(
    l(t('Home'), '<front>'),
    l(t('Administration'), 'admin'),
    l(t('Content'), 'admin/content'),
    l(t('Assets'), 'admin/content/assets'),
  );

  drupal_set_breadcrumb($breadcrumb);
}