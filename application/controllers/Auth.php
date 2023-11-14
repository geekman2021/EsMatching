<?php 

    class Auth extends CI_Controller {
        public function __construct() {
            parent::__construct();
            $this->load->model("Check_Model");
            $this->load->model("User_Model");
            $this->load->library("form_validation");
            session_start();
        }

        public function index() {
            $this->load->view("/pages/login");
        }

        public function login() {

            $this->form_validation->set_rules("num_mat", "Numero Matricule introuvalble", "trim|required");
            $this->form_validation->set_rules("password", "Mot de passe incorrect", "required");

            if($this->form_validation->run() == FALSE) {
                $this->load->view("pages/login");
            } else {
                if(isset($_POST["num_mat"]) && isset($_POST["password"])) {
                    if($this->Check_Model->loginAD($_POST["num_mat"], $_POST["password"])){
                       
                        if($this->Check_Model->checkMatriculeIfExist($_POST["num_mat"])){
                            $_SESSION["isLogin"]= true;
                            $_SESSION["userExist"]= $this->User_Model->get_userBynumMat($_POST["num_mat"]);
                            redirect("/dashboard");
                        }
                    }

                } else {
                    $data["login_fail"] = "Erreur de connexion à LDAP";
                    $this->load->view("auth/login", $data);
                }
            }
            $num_mat= $this->input->post("num_mat");
            $password= $this->input->post("password");
        }

        public function logOut() {
            session_destroy();
            redirect("auth/login");
        }

    }


?>