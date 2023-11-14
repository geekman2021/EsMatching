<?php
    class Reference_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        // public function insert($data) {
        //     return $this->db->insert("reference", $data);
        // }

        public function get_telma() {
            $query = $this->db->get("reference");
            if ($query->num_rows() > 0) {
                return $query->row()->reference_telma;
            } else {
                return 0; 
            }

        } 

        public function get_orange() {
            $query = $this->db->get("reference");
            if ($query->num_rows() > 0) {
                return $query->row()->reference_orange;
            } else {
                return 0; 
            }

        }
        public function get_airtel() {
            $query = $this->db->get("reference");
            if ($query->num_rows() > 0) {
                return $query->row()->reference_airtel;
            } else {
                return 0; 
            }

        }
        public function incrementerReferenceTelma() {
            $dernierNumero = $this->get_telma();
            $nouveauNumero = $dernierNumero + 1;
            
            $this->db->update('reference', array('reference_telma' => $nouveauNumero));
            return $nouveauNumero;
        }

        public function incrementerReferenceOrange() {
            $dernierNumero = $this->get_orange();
            $nouveauNumero = $dernierNumero + 1;
            
            $this->db->update('reference', array('reference_orange' => $nouveauNumero));
            return $nouveauNumero;
        }

        public function incrementerReferenceAirtel() {
            $dernierNumero = $this->get_airtel();
            $nouveauNumero = $dernierNumero + 1;
            
            $this->db->update('reference', array('reference_airtel' => $nouveauNumero));
            return $nouveauNumero;
        }


    }
?>