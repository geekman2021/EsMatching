<div class="container mt-4">
    <div class="card shadow mb-4">
        <div class="card-header py-3">
            <h6 class="m-0 font-weight-bold" style="color: #11101d">Liste d'utilisateur</h6>
        </div>
        <div class="card-body">
            <table class="table table-bordered">
                <thead style="text-align: center;">
                    <tr >
                        <th style="display: none;">Id </th>
                        <th>Code Profile</th>
                        <th>Id Droit</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody style="text-align: center;">
                    <?php foreach ($data as $droit_profile) { ?>
                        <tr>
                            <td style="display: none;"><?php echo $droit_profile->id; ?></td>
                            <td><?php echo $droit_profile->code_profil; ?></td>
                            <td><?php echo $droit_profile->id_droit; ?></td>
                            <td style="text-align: center ;">
                                <form action="" method="DELETE">
                                    <a class="btn btn-info" href="#" onclick="modifier('<?php echo $droit_profile->id; ?>', '<?php echo $droit_profile->code_profil; ?>', '<?php echo $droit_profile->id_droit; ?>')">Modifier</a>
                                    <button class="btn btn-danger" onclick="supprimer('<?php echo $droit_profile->id; ?>')">Supprimer</button>
                                </form>
                            </td>
                        </tr>
                    <?php }; ?>
                </tbody>
            </table>
        </div>
    </div>
</div>




<script type="text/javascript">
    function modifier(id, code_profile, id_droit) {
        document.getElementById("code_profile").value= code_profile;
        document.getElementById("id_droit").value= id_droit;
        document.getElementById("id").value = id;
    }

    function supprimer(id) {
        var confirm= window.confirm("Voulez-vous vraiment supprimer");

        if (confirm) {
            $.ajax({
                url: "<?php echo site_url("droit_profil/remove_droit_user") ?>",
                type: "POST",
                data: {
                    "id": id
                },
                success: function (response) {
                    window.location.reload();
                },
                error: function (error) {
                    alert(error);
                }
            });
        }
       
    }


   $("#addForm").submit(function(e) {

    e.preventDefault();
    var code_profile= $("#code_profile").val();
    var id_droit= $("#id_droit").val();
    var id = $("#id").val();

    if(id) {
        $.ajax({
                url: "<?php echo site_url("droit_profil/edit_droit_user") ?>",
                type: "PUT",
                data: JSON.stringify({
                    id: id,
                    code_profile: code_profile,
                    id_droit: id_droit
                }),
                success: function(response) {
                    window.location.reload();
                },
                error: function(error) {
                    console.log(error);
                }
            });
        
    }else {
        $.ajax({
                url: "<?php echo site_url("droit_profil/add_droit_profil") ?>",
                type: "POST",
                data: {
                    "code_profile": code_profile,
                    "id_droit": id_droit
                },
                success: function(response) {
                    window.location.reload();
                },
                error: function(error) {
                    console.log(error);
                }
            });
    }
   });
</script






    
