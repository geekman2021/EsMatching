<?php
class Droits extends CI_Controller {

    public $droitCRUD;
    
    public function __construct(){
        parent::__construct();
        $this->load->model("Droit_Model");
        $this->droitCRUD= new Droit_Model;
        
    }

    public function header () {
        header("Access-Control-Allow-Origin: *");
        header("Access-Control-Allow-Methods: PUT, POST, GET, DELETE, OPTIONS");
        header("Access-Control-Allow-Headers: Content-Type");
    }

    public function index() {
        $data["data"] = $this->droitCRUD->get_droit();
        $this->load->view("templates/sidebar");
        $this->load->view("pages/droit/droit_form");
        $this->load->view("pages/droit/droit_list", $data);
        $this->load->view("templates/footer");
    }

    public function create() {
        $code_droit= $this->input->post("code_droit");
        $permission= $this->input->post("permission");

        $this->droitCRUD->insert(array(
            "code_droit" => $code_droit,
            "permission" => $permission
        ));
    }

    public function edit() {
        $this->header();
        $putdata = $this->input->raw_input_stream;
        $request = json_decode($putdata);
        $id= $request->id;
        $code_droit = $request->code_droit;
        $permission = $request->permission;

        $this->droitCRUD->update($id, array(
            "code_droit" => $code_droit,
            "permission" => $permission
        ));        
    }

    public function remove() {
        $id= $this->input->post("id");
        $this->droitCRUD->delete($id);
    }
 
}

?>