
<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>
<div class="container mt-4">
    <div class="card shadow mb-4">
        <div class="card-header py-3">
            <h6 class="m-0 font-weight-bold" style="color: #11101d">Liste Utilisateurs</h6>
        </div>
        <div class="card-body">
            <div class="container mt-4">
                <table class="table table-bordered" id="userList">
                    <thead>
                        <tr >
                            <th style="display: none;">Id </th>
                            <th>Matricule</th>
                            <th>Nom</th>
                            <th>Profil</th>
                            <th>Action</th>
                        </tr>
                    </thead>
                    <tbody>
                    <?php if(!empty($data))  { ?>
                        <?php foreach($data as $user) { ?>
                            <tr>
                                <td style="display: none;"><?= $user->id_utilisateur ?> </td>
                                <td><?= $user->matricule ?></td>
                                <td><?= $user->nom ?></td>
                                <td><?= $user->libelle ?></td>  
                                <td>
                                    <a href="#" class="btn btn-warning" onClick="editUser('<?= $user->id_utilisateur ?>', '<?= $user->matricule ?>', '<?= $user->nom ?>', '<?= $user->code_profile ?>')">Modifier</a>
                                    <a href="#" class="btn btn-danger" onClick="supprimer('<?= $user->id_utilisateur ?>')">Supprimer</a>
                                </td>
                            </tr>
                        <?php }?>
                    <?php }?>
                    </tbody>
                </table>
            </div>
        </div>
    </div>
</div>

<script>
    function editUser(id, num_matricule, nom, code_profile) {
        $("#id").val(id); 
        $("#num_mat").val(num_matricule);
        $("#nom").val(nom);
        $("#code_profile").val(code_profile);
    }
    $("#addformUser").submit(function(e) {
        e.preventDefault();
        const id= $("#id").val();
        const num_mat= $("#num_mat").val();
        const nom= $("#nom").val();
        const code_profile= $("#code_profile").val();

        if(id) {
            $.ajax({
                url: "<?php echo site_url("users/edit_user") ?>",
                type: "POST",
                data: {
                    "id": id,
                    "num_mat": num_mat,
                    "nom": nom,
                    "code_profile": code_profile
                },
                success: function (response) {
                    window.location.reload();
                    // console.log(response);
                },
                error: function (error) {
                    alert(error);
                }
            });
            
        } else {
            $.ajax({
                url: "<?php echo site_url("users/add_user") ?>",
                type: "POST",
                data: {
                    "num_mat": num_mat,
                    "nom": nom,
                    "code_profile": code_profile
                },
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            });
        }
            

      

    });

    $(document).ready(function() {
        $('#userList').DataTable( {
        language: {
            url: '<?php echo base_url(); ?>assets/fr-FR.json',
        }
        });
    });
</script>











    
