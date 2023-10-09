<?php

    class Droit_Profil extends CI_Controller {

        // public $droitCRUD;
        // public $profileList;
        // public $droitList;
        public function __construct(){
            parent::__construct();
            $this->load->model("Droit_Profil_Model");
            $this->load->model("Profil_Model");
            $this->load->model("Droit_Model");
            // $this->droitCRUD= new Droit_Profil_Model;
            // $this->profileList= new Profil_Model;
            // $this->droitList= new Droit_Model;
        }

        public function index() {

            $data["data"] = $this->Droit_Profil_Model->get_droits_profile();
            $data["profil"] = $this->Profil_Model->get_profils();
            $data["droit"] = $this->Droit_Model->get_droit();
            $this->load->view("templates/sidebar");
            $this->load->view("pages/droit-profile/droit-profil-form", $data);
            $this->load->view("pages/droit-profile/droit-profil-list", $data);
            $this->load->view("templates/footer");
        }

        public function add_droit_profil() {

            $code_profil= $this->input->post("code_profile");
            $id_droit= $this->input->post("id_droit");

            $this->Droit_Profil_Model->insert(array(
                "code_profil" =>$code_profil,
                "id_droit" => $id_droit
            ));
        }
        public function remove_droit_user() {
            $id= $this->input->post("id");
            $this->Droit_Profil_Model->delete($id);
        }

        public function edit_droit_user() {
            $putdata = $this->input->raw_input_stream;
            $request= json_decode($putdata);

            $id= $request->id;
            $code_profil= $request->code_profile;
            $id_droit= $request->id_droit;
            $this->Droit_Profil_Model->update($id, array(
                "code_profil" => $code_profil,
                "id_droit" => $id_droit
            ));
        }

    }

?>