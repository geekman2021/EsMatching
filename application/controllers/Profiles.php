<?php
    class Profiles extends CI_Controller {

        
        public function header () {
            header("Access-Control-Allow-Origin: *");
            header("Access-Control-Allow-Methods: PUT, POST, GET, DELETE, OPTIONS");
            header("Access-Control-Allow-Headers: Content-Type");
        }

        public function __construct() {
            parent::__construct();
            $this->load->model("Profil_Model");
            session_start();
        }

        public function index() {
            $data["data"] = $this->Profil_Model->get_profils(); 
            $this->load->view("templates/sidebar");
            $this->load->view("pages/profile/profile_form", $data);
            $this->load->view("pages/profile/profile_list", $data);
            $this->load->view("templates/footer");
        }

        public function create() {
            // $this->header();
            $code_profile= $this->input->post("code_profile");
            $libelle= $this->input->post("libelle");

            $this->Profil_Model->insert_profil(array(
                "code_profile" => $code_profile,
                "libelle" => $libelle
            ));
        }

        public function edit() {
            $this->header();
            $putdata = $this->input->raw_input_stream;
            $request = json_decode($putdata);
            $id = $request->id;
            $code_profile= $request->code_profile;
            $libelle = $request->libelle;
            
            $this->Profil_Model->update_profil($id, array(
                "code_profile" => $code_profile,
                "libelle" => $libelle,
            ));
        }

        public function remove() {
            $this->header();
            $id= $this->input->post("id");

            $this->Profil_Model->delete_profil($id); 
        }


        // public function edit($id) {
        //     $this->load->view("templates/header.php"); 
        //     $this->load->view("pages/user_form");
        //     $this->load->view("pages/user_list", $data);
        //     $this->load->view("templates/footer");
        // }
    }
    
?>