<?php
    class Users extends CI_Controller {

        public $user_model;
        public $profil_model;

        public function header() {
            header("Access-Control-Allow-Origin: *");
            header("Access-Control-Allow-Methods: PUT, POST, GET, DELETE, OPTIONS");
            header("Access-Control-Allow-Headers: Content-Type");
        }

        public function __construct() {
            parent::__construct();
            $this->load->model("user_model");
            $this->load->model("profil_model");
        }

        public function index() {

        
            // $this->header();

            $data["data"] = $this->user_model->get_users(); 
            $data["profile"] = $this->profil_model->get_profils();
            $this->load->view("templates/sidebar");
            $this->load->view("pages/users/user_form", $data);
            $this->load->view("pages/users/user-list");
            // $this->load->view("templates/footer");
        }

        public function add_user() {

            // $this->header();
            $num_mat = $this->input->post('num_mat');
            $nom = $this->input->post('nom');
            $code_profile = $this->input->post('code_profile');
            $code_unite= $this->input->post("code_unite");
            $this->user_model->insert_user(array(
                "matricule" => $num_mat,
                "code_profil" => $code_profile,
                "nom" => $nom,
                "code_unite" => $code_unite
            ));
        }

        public function edit_user() {

            $this->header();
            $putdata = $this->input->raw_input_stream;
            $request = json_decode($putdata);
            $id = $request->id;
            $num_mat = $request->num_mat;
            $nom = $request->nom;
            $code_profile = $request->code_profile;
            $code_unite = $request->code_unite;

            $this->user_model->update_user($id, array(
                "matricule" => $num_mat,
                "code_profil" => $code_profile,
                "nom" => $nom,
                "code_unite" => $code_unite
            ));
            // redirect(site_url("/users"));
        }

        public function remove_user() {
            $this->header();
            $id = $this->input->post("id");    
            $this->user_model->delete_user($id);
        }


        // public function edit($id) {
        //     $this->load->view("templates/header.php"); 
        //     $this->load->view("pages/user_form");
        //     $this->load->view("pages/user_list", $data);
        //     $this->load->view("templates/footer");
        // }
    }
    
?>