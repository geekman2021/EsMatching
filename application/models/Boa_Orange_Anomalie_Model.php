<?php

    class Boa_Orange_Anomalie_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update_dat($data) {
            $this->db->where("comm_ref_igor", $data["comm_ref_igor"]);
            $exist= $this->db->get("boa_orange_anomalie_ci");

            if($exist->num_rows() == 0 ){
                return $this->db->insert("boa_orange_anomalie_ci", $data);
            } else {
                $this->db->where("comm_ref_igor", $data["comm_ref_igor"]);
                return $this->db->update("boa_orange_anomalie_ci", $data);
            }
        } 

        public function insert_or_update_cat($data) {
            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_anomalie_co");

            if($exist->num_rows() == 0 ){
                return $this->db->insert("boa_orange_anomalie_co", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_anomalie_co", $data);
            }
        }

        public function insert_or_update_vi($data) {
            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_vi");

            if($exist->num_rows() == 0 ){
                return $this->db->insert("boa_orange_vi", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_vi", $data);
            }
        }

        public function insert_or_update_nonAuCI($data) {
            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_nonau");

            if($exist->num_rows() ==0 ){
                return $this->db->insert("boa_orange_nonau", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_nonau", $data);
            }
        } 
        public function insert_or_update_nonAuCO($data) {
            $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
            $exist= $this->db->get("boa_orange_nonau");

            if($exist->num_rows() ==0 ){
                return $this->db->insert("boa_orange_nonau", $data);
            } else {
                $this->db->where("princ_ref_igor", $data["princ_ref_igor"]);
                return $this->db->update("boa_orange_nonau", $data);
            }
        }

        public function get_vi() {
            $this->db->select("*");
            $this->db->from("boa_orange_vi");
            $query= $this->db->get();

            return $query->result();
        }


        public function get_ci() {
            $this->db->select("*");
            $this->db->from("boa_orange_anomalie_ci");
            $query= $this->db->get();

            return $query->result();
        }

        public function get_co() {
            $this->db->select("*");
            $this->db->from("boa_orange_anomalie_co");
            $query= $this->db->get();

            return $query->result();
        }
    }

?>