<?php
    class Profil_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("db1");
        }

        public function insert_profil($data) {
            return $this->db->insert("profils", $data);
        }

        public function get_profils() {

            $this->db->select("*");
            $this->db->from("profils");
            $this->db->order_by("code_profile", "ASC");

            $query= $this->db->get();
            return $query->result();
        }
        public function delete_profil($id) {
            $this->db->where("id", $id);
            $this->db->delete("profils");
        }

        public function update_profil($id, $data) {
            $this->db->where("id", $id);
            return $this->db->update("profils", $data);
        }

        public function find_profile($id) {
            return $this->db->get_where("profiles", array("id" =>$id))->row();
        }
    }
?>