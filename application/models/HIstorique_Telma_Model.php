<?php
    
    class Historique_Telma_Model extends CI_Model {
        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        
        public function insert($data) {
            $this->db->insert("historique_telma", $data);
        }

        public function get_historique() {
            $this->db->select("*");
            $this->db->from("historique_telma");
            $this->db->order_by("id", "ASC");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_last_solde() {
            $sql= "SELECT solde_telma, solde_boa FROM historique_telma ORDER BY id DESC LIMIT 1";

            $query= $this->db->query($sql);

            return $query->result();
        }


    }


?>