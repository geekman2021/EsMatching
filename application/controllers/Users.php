<?php
    class Users extends CI_Controller {

        public function __construct() {
            parent::__construct();
            $this->load->model("User_Model");
            $this->load->model("Profil_Model");
            session_start();
        }

        public function index() {

            $data["data"] = $this->User_Model->get_users(); 
            $data["profile"] = $this->Profil_Model->get_profils();
            $this->load->view("templates/sidebar");
            $this->load->view("pages/users/user_form", $data);
            $this->load->view("pages/users/user-list", $data);
        }

        public function add_user() {

            $num_mat = $this->input->post('num_mat');
            $nom = $this->input->post('nom');
            $code_profile = $this->input->post('code_profile');
            $this->User_Model->insert_user(array(
                "matricule" => $num_mat,
                "code_profil" => $code_profile,
                "nom" => $nom,
            ));
        }

        public function edit_user() {
            $update_data= array(
                "matricule" => $this->input->post("num_mat"),
                "code_profil" => $this->input->post("code_profile"),
                "nom" => $this->input->post("nom")
            );

            $this->User_Model->update_user($this->input->post("id"),$update_data );
        }

        public function remove_user() {
            $id = $this->input->post("id");    
            $this->User_Model->delete_user($id);
        }


        // public function edit($id) {
        //     $this->load->view("templates/header.php"); 
        //     $this->load->view("pages/user_form");
        //     $this->load->view("pages/user_list", $data);
        //     $this->load->view("templates/footer");
        // }
    }
    
?>