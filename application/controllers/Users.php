<?php
    class Users extends CI_Controller {

        public $usersCRUD;
        public $profilList;

        public function header() {
            header("Access-Control-Allow-Origin: *");
            header("Access-Control-Allow-Methods: PUT, POST, GET, DELETE, OPTIONS");
            header("Access-Control-Allow-Headers: Content-Type");
        }

        public function __construct() {
            parent::__construct();
            // $this->load->model("User_Model");
            // $this->load->model("Profil_Model");
            // $this->usersCRUD= new User_Model;
            // $this->profilList= new Profil_Model;
            
        }

        public function index() {

            echo "bonjour";
            $this->load->view("templates/header");
            // $this->header();

            // $data["data"] = $this->usersCRUD->get_users(); 
            // $data["profile"] = $this->profilList->get_profils();
            // $this->load->view("templates/sidebars");
            // $this->load->view("pages/users/user_form", $data);
            // $this->load->view("pages/users/user_list", $data);
            // $this->load->view("templates/footer");
        }

        public function add_user() {

            $this->header();
            $num_mat = $this->input->post('num_mat');
            $nom = $this->input->post('nom');
            $code_profile = $this->input->post('code_profile');
            
            $this->usersCRUD->insert_user(array(
                "num_mat" => $num_mat,
                "code_profile" => $code_profile,
                "nom" => $nom,
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

            $this->usersCRUD->update_user($id, array(
                "num_mat" => $num_mat,
                "code_profile" => $code_profile,
                "nom" => $nom,
            ));
            // redirect(site_url("/users"));
        }

        public function remove_user() {
            $this->header();
            $id = $this->input->post("id");    
            $this->usersCRUD->delete_user($id);
        }


        // public function edit($id) {
        //     $this->load->view("templates/header.php"); 
        //     $this->load->view("pages/user_form");
        //     $this->load->view("pages/user_list", $data);
        //     $this->load->view("templates/footer");
        // }
    }
    
?>