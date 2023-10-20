<?php

    class Orange_Anomalie_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_ci($data) {
            $this->db->where("orange_ref", $data["orange_ref"]);
            $exist= $this->db->get("orange_anomalie_ci");

            if($exist->num_rows() ==0 ){
                return $this->db->insert("orange_anomalie_ci", $data);
            } else {
                $this->db->where("orange_ref", $data["orange_ref"]);
                return $this->db->update("orange_anomalie_ci", $data);
            }
        } 

        
        public function insert_or_update_co($data) {
            $this->db->where("orange_ref", $data["orange_ref"]);
            $exist= $this->db->get("orange_anomalie_co");

            if($exist->num_rows() == 0 ){
                return $this->db->insert("orange_anomalie_co", $data);
            } else {
                $this->db->where("orange_ref", $data["orange_ref"]);
                return $this->db->update("orange_anomalie_co", $data);
            }
        }

        public function insert_or_update_ind($data) {
            $this->db->where("orange_ref", $data["orange_ref"]);
            $exist= $this->db->get("orange_ind");

            if($exist->num_rows() == 0 ){
                return $this->db->insert("orange_ind", $data);
            } else {
                $this->db->where("orange_ref", $data["orange_ref"]);
                return $this->db->update("orange_ind", $data);
            }
        }

        public function get_ci() {
            $this->db->select("*");
            $this->db->from("orange_anomalie_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("orange_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_ind() {
            $this->db->select("*");
            $this->db->from("orange_ind");
            $query= $this->db->get();

            return $query->result();
        }
    }

?>