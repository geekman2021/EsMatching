<?php
    class Non_Au_Model extends CI_Model {

        public function __construct() {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_or_update($data) {

            $this->db->where("REF_IGOR", $data["REF_IGOR"]);
            $exist= $this->db->get("nonau");

            if($exist->num_rows() == 0) {
                return $this->db->insert("nonau", $data);
            } else {
                $this->db->where("REF_IGOR", $data["REF_IGOR"]);
                return $this->db->update("nonau", $data);
            }
        }  
    }

?>