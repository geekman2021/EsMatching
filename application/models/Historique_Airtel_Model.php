<?php
    
    class Historique_Airtel_Model extends CI_Model {
        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        
        public function insert($data) {
            $this->db->insert("historique_airtel", $data);
        }

        public function get_historique() {
            $this->db->select("*");
            $this->db->from("historique_airtel");
            $this->db->order_by("id", "ASC");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_last_solde() {
            $sql= "SELECT solde_airtel, solde_boa FROM historique_airtel ORDER BY id DESC LIMIT 1";
            $query= $this->db->query($sql);

            return $query->result();
        }


    }


?>