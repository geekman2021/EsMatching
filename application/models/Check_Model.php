<?php
    class Check_Model extends CI_Model {

        public function __construct(){
            parent::__construct();
            $this->load->database("db1");
            
        }

        public function loginAD($username, $password) {
            // $ldapconn_1 = @ldap_connect('ldap://' ."server_url");
            // $ldapbind_1 = @ldap_bind($ldapconn_1, $username . '@' . "nom_domaine_ad1", $password);
        
            // $ldapconn_2 = @ldap_connect('ldap://' ."server2_url");
            // $ldapbind_2 = @ldap_bind($ldapconn_2, $username . '@' . "nom_domaine_ad2", $password);
        
            // $ldapconn_3 = @ldap_connect('ldap://' ."server3_url");
            // $ldapbind_3 = @ldap_bind($ldapconn_3, $username . '@' . "nom_domaine_ad2", $password);
        
            // return ($ldapbind_1 or $ldapbind_2 or $ldapbind_3);
            return true;
        }

        public function checkMatriculeIfExist($num_matricule) {
            $user= $this->db->get_where("utilisateurs", array("num_mat" => $num_matricule));
            return !($user->row_array()==null);
        }
    
    
}


?>