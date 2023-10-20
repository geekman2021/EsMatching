<?php
    class User_Model extends CI_Model{
        public function __construct()
        {
            parent::__construct();
            $this->load->database("default");
        }

        public function insert_user($data) {
            return $this->db->insert("utilisateurs", $data);
        }

        public function get_users() {

            $this->db->select("*");
            $this->db->from("utilisateurs");
            $this->db->order_by("code_profil", "ASC");

            $query= $this->db->get();
            return $query->result();
        }
        public function delete_profil($id) {
            $this->db->where("id", $id);
            $this->db->delete("utilisateurs");
        }

        public function update_profil($id, $data) {
            $this->db->where("id", $id);
            return $this->db->update("utilisateurs", $data);
        }

        public function find_profile($id) {
            return $this->db->get_where("utilisateurs", array("id" =>$id))->row();
        }
    }
?>