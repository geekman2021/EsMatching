<form id="addform">
    <div class="container mt-5 container-parent-form">
        <div class="row">
        <div class="form-group col-lg-6" style="display: none;">
                <label for="num_mat">id:</label>
                <input type="text" class="form-control" id="id">
            </div>
            <div class="form-group col-lg-6">
                <label for="num_mat">Num Matricule:</label>
                <input type="text" class="form-control" id="num_mat" required>
            </div>
            <div class="form-grou col-lg-6">
                <label for="nom">Nom:</label>
                <input type="text" class="form-control" id="nom" required>
            </div>
        </div>
        <div class="form-group">
            <label for="">Profile</label>
            <select class="form-control" id="code_profile">
                <?php foreach($profile as $p) { ?>
                    <option value="<?php echo $p->code_profile; ?>"><?php echo $p->libelle; ?></option>
                <?php }; ?>
            </select>
        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-warning" id="btn-reinitialiser"><i class="fa-solid fa-rotate" style="margin-right: 5px;"></i>Reinitialiser</button>
                <button type="submit" class="btn btn-success" id="btn-ajouter"> <i class="fa-solid fa-plus" style="margin-right: 5px;"></i>Ajouter</button>
            </div>
        </div>
    </div>
</form>